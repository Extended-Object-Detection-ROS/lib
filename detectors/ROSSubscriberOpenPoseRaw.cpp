#if (USE_ROS)

#include "ROSSubscriberOpenPoseRaw.h"
#include "geometry_utils.h"

// Almost all logic is inspired by https://github.com/kschlegel/OAK-HumanPoseEstimation/blob/main/poseestimators/openpose.py


namespace eod{
    
    cv::Mat get_mat_from_data(const std::vector<float> &data, int channel, int width, int height){
        auto start = data.begin() + channel * height * width;
        auto end = data.begin() + (1+channel) * height * width;
                            
        cv::Mat channel_mat(height, width, CV_64F, std::vector<double>(start, end).data() );
        return channel_mat;        
    }
    
        
    ROSSubscriberOpenPoseRaw::ROSSubscriberOpenPoseRaw(std::string topic_name, float timelag) : ROSSubscriberBaseAttribute(topic_name, timelag){
        Type = ROS_SUB_OPENPOSE_RAW_A;        
    }
    
    std::vector<ExtendedObjectInfo> ROSSubscriberOpenPoseRaw::Detect2(const InfoImage& image, int seq){
        ros::Time now = ros::Time::now();
        ros::Time imtime = ros::Time(image.sec_, image.nsec_);
        std::vector<ExtendedObjectInfo> results;        
            
        ExtendedObjectInfo tmp(0, 0, 2, 2);
        
        auto msg = cache_->getElemAfterTime(imtime);
        int num_keypoints;
        do{                        
            if( msg == nullptr ){
                //printf("waiting\n");
                msg = cache_->getElemAfterTime(imtime);
            }
            
            else{
                //printf("yes msg\n");
                //printf("diff is %f\n", (imtime - cache_->getLatestTime()).toSec() );
                
                // PROCESS DATA
                std::vector<std::vector<std::pair<cv::Point, double>>> keypoints; // channel vise keypoints
                cv::Size proc_size;
                // get keypoints
                for( const auto &layer : msg->layers){
                    if( layer.name == "heatmaps" ){
                        int width, height;
                        for( const auto& dim : layer.tensor.layout.dim ){
                            if( dim.label == "channel" )
                                num_keypoints = dim.size;
                            if( dim.label == "height" )
                                height = dim.size;
                            if( dim.label == "width" )
                                width = dim.size;
                        }
                        proc_size = cv::Size(width, height);
                        for( size_t i = 0 ; i < num_keypoints; i++){                        
                            
                            cv::Mat channel = get_mat_from_data(layer.tensor.data, i, width, height);
                            
                            keypoints.push_back(get_keypoints_single_channel(channel));
                            
                            //cv::Mat out;
                            //cv::normalize(channel, out, 0, 1, cv::NORM_MINMAX);                                                        
                            //cv::resize(out, out, cv::Size(msg->input_width, msg->input_height) );
                            
                            /*
                             for( const auto& kpt_ch : keypoints){
                                 for( const auto& kpt : kpt_ch ){
                                    std::string label = landmarks_labels.size() > i ? landmarks_labels[i] : ""; 
                                    tmp.keypoints.push_back(eod::KeyPoint(scale_point(kpt.first, proc_size, image.size()), kpt.second, label));
                                    printf("%i %i\n", tmp.keypoints.back().x, tmp.keypoints.back().y);
                                 }
                             }
                             */
                        }                        
                    }
                }
                
                // get connections
                printf("Connection\n");
                std::vector<std::vector<std::pair<cv::Point, double>>> pairs;
                for( const auto &layer : msg->layers){
                    if( layer.name == "pafs" ){
                        int height, width;
                        for( const auto& dim : layer.tensor.layout.dim ){
//                             if( dim.label == "channel" )
//                                 num_keypoints = dim.size;
                            if( dim.label == "height" )
                                height = dim.size;
                            if( dim.label == "width" )
                                width = dim.size;
                        }
                        
                        // this pairs grouped by connections, each member is indexes of keypoits in theirs channels and score of connection
                        
                        
                        for( const auto& connection_pafs_ids : paf_keys ){
                            //printf("Next con\n");
                            auto connection = connection_pafs_ids[0];
                            auto pafs_ids = connection_pafs_ids[1];
                            
                            //printf("Data get\n");
                            cv::Mat paf_a = get_mat_from_data(layer.tensor.data, pafs_ids[0], width, height);
                            cv::Mat paf_b = get_mat_from_data(layer.tensor.data, pafs_ids[1], width, height);
                            //printf("Data got\n");
                            
                            std::vector<std::pair<cv::Point, double>>* cand_a = &(keypoints[connection[0]]);
                            std::vector<std::pair<cv::Point, double>>* cand_b = &(keypoints[connection[1]]);
                                                        
                            std::vector<std::pair<cv::Point, double>> pairs_found;
                            
                            
                            for( size_t i = 0 ; i < cand_a->size() ; i++){
                                //printf("Next i\n");
                                fflush(stdout);
                                
                                int max_j = -1;
                                double max_score = -1;
                                
                                for( size_t j = 0 ; j < cand_b->size() ; j++){
                                    //printf("Next j\n"); 
                                    fflush(stdout);
                                    
                                    double unit_a2b_x = cand_b->at(j).first.x - cand_a->at(i).first.x;
                                    double unit_a2b_y = cand_b->at(j).first.y - cand_a->at(i).first.y;
                                    
                                    double vec_norm = std::sqrt( unit_a2b_x * unit_a2b_x + unit_a2b_y * unit_a2b_y); 
                                                                                                            
                                    if( vec_norm > 0){                          
                                        unit_a2b_x /= vec_norm;
                                        unit_a2b_y /= vec_norm;
                                    }
                                    else{
                                        //printf("NOES\n");
                                        continue;
                                    }
                                                       
                                    
                                    cv::Mat paf_samples = cv::Mat::zeros(cv::Size(_num_paf_samples, 2), CV_64F);
                                    
                                    //printf("Even this?\n");                                    
                                    double x_len = cand_b->at(j).first.x - cand_a->at(i).first.x;
                                    double y_len = cand_b->at(j).first.y - cand_a->at(i).first.y;
                                    
                                    
                                    for( int k = 0 ; k < _num_paf_samples ; k++ ){
                                        double coord_x = cand_a->at(i).first.x + (x_len / (_num_paf_samples - 1)) * k;
                                        double coord_y = cand_a->at(i).first.y + (y_len / (_num_paf_samples - 1)) * k;
                                                               
                                        int x = int(std::round(coord_x));
                                        int y = int(std::round(coord_y));                                                                                
                                        
                                        if( x >= width )
                                            x = width-1;
                                        if( y >= height )
                                            y = height-1;
                                                                                                                                            
                                        //paf_samples.at<double>(k, 0) = paf_a.at<double>(x, y);
                                        //paf_samples.at<double>(k, 1) = paf_b.at<double>(x, y);
                                        paf_samples.at<double>(0, k) = paf_a.at<double>(y, x);
                                        paf_samples.at<double>(1, k) = paf_b.at<double>(y, x);
                                        
                                    }                                                          
                                    //printf("Latir\n");                                    
                                    
                                    std::vector<double> paf_scores;
                                    for( int k = 0 ; k < _num_paf_samples ; k++ ){
                                        double dot_prod = paf_samples.at<double>(0, k) * unit_a2b_x + paf_samples.at<double>(1, k) * unit_a2b_y;
                                        paf_scores.push_back(dot_prod);
                                        //printf("Dot_prod %f\n", dot_prod);
                                    }
                                    
                                    int good_paf_scores = 0;
                                    double sum_scores = 0;
                                    for( int k = 0 ; k < _num_paf_samples; k++){
                                        if( paf_scores.at(k) > _min_paf_score_th)
                                            good_paf_scores++;
                                        sum_scores += paf_scores.at(k);
                                    }
                                    
                                    double mean_good_paf_score = double(good_paf_scores) / _num_paf_samples;
                                    //printf("Mean good paf score %f\n", mean_good_paf_score);
                                    if( mean_good_paf_score  > _paf_sample_th ){
                                        double avg_paf_score = sum_scores / _num_paf_samples;
                                        //printf("avg %f\n", avg_paf_score);
                                        if( avg_paf_score > max_score){
                                            max_j = j;
                                            max_score = avg_paf_score;
                                        }
                                    }            
                                    //printf("Cycle end, max_j %i, max_score %f\n", max_j, max_score);
                                    
                                }                                
                                if( max_j >= 0 ){                                    
                                    pairs_found.push_back(std::make_pair(cv::Point(i, max_j), max_score));                                    
                                }
                                //printf("Real cycle end\n");
                            }
                            //printf("Real i cycle end\n");
                            pairs.push_back(pairs_found);
                            //printf("Pushed back\n");
                        }
                        
                    }
                }
                        
                
                //printf("TESTING CONNECTION\n");
                /*
                for( size_t i = 0 ; i < paf_keys.size() ; i++ ){
                    
                    auto connection = paf_keys[i][0];
                    //printf("Connection problem?\n");
                    
                    for( auto& pair : pairs[i] ){
                        //printf("Next pair\n");
                        
                        auto kpt_ind_1 = pair.first.x;
                        auto kpt_ind_2 = pair.first.y;
                        
                        auto kpt_1 = keypoints[connection[0]][kpt_ind_1];
                        auto kpt_2 = keypoints[connection[1]][kpt_ind_2];
                        
                        tmp.keypoints.push_back(eod::KeyPoint(scale_point(kpt_1.first, proc_size, image.size()), kpt_1.second, ""));
                        
                        tmp.keypoints.push_back(eod::KeyPoint(scale_point(kpt_2.first, proc_size, image.size()), kpt_2.second, ""));
                        
                        tmp.keypoint_connection.push_back(std::make_pair(tmp.keypoints.size()-1, tmp.keypoints.size()-2));                                                
                    }                                        
                }
                */
                                                                
                // get personwise                                 
                //std::vector<std::map<int, std::vector<double>>> persons;
                //std::vector<std::vector<std::vector<double>>> persons;
                
                std::vector<std::vector<int>> persons;                
                for( auto person : persons){
                    person = std::vector<int>(num_keypoints);
                    for( auto val : person )
                        val = -1;
                }
                
                
                
//                 for( size_t connection_id = 0 ; connection_id < paf_keys.size() ; connection_id++ ){
//                     
//                     if( pairs[connection_id].size() == 0 )
//                         continue;
//                     
//                     int start_landmark = paf_keys[connection_id][0][0];
//                     int end_landmark = paf_keys[connection_id][0][1];
//                     
//                     for( size_t i = 0 ; i < pairs[connection_id].size() ; i++ ){
//                         int start_keypoint = pairs[connection_id][i].first.x; // id of KeyPoint in keypoints for this connection_id
//                         int end_keypoint = pairs[connection_id][i].first.y;
//                         
//                         int person_id = -1;
//                         for( size_t j = 0 ; j < persons.size() ; j++ ){
//                             persons[j]                            
//                         }
//                         
//                     }                                    
//                 }
                
                
                tmp.setScoreWeight(1,1);
                results.push_back(tmp);
                                
                return results;
            }                        
            
        }  
        while( (ros::Time::now() - now).toSec() < timelag_ && ros::ok());
        
        printf("fin\n");
        //printf("diff is %f\n", (imtime - cache_->getLatestTime()).toSec() );
        return results;
    }        
    
    void ROSSubscriberOpenPoseRaw::Extract2(const InfoImage& image, ExtendedObjectInfo& rect){
    }
    
    bool ROSSubscriberOpenPoseRaw::Check2(const InfoImage& image,ExtendedObjectInfo& rect){
        return false;
    }  
    
    std::vector<std::pair<cv::Point, double>> ROSSubscriberOpenPoseRaw::get_keypoints_single_channel(const cv::Mat& heatmap_channel){
        
        std::vector<std::pair<cv::Point,double> > channel_keypoints;
        
        // filter by prob        
        cv::Mat mask;
        cv::inRange(heatmap_channel, Probability, 1, mask);
        
        // find clusters
        std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours( mask, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE );
        
        // get cluster centers
        for( const auto& contour : contours ){
            cv::Mat contour_mask = cv::Mat::zeros(heatmap_channel.size(), CV_8UC1);
            cv::fillConvexPoly(contour_mask, contour, 1);
            double max, min;
            cv::Point maxLoc, minLoc;
            cv::minMaxLoc(heatmap_channel, &min, &max, &minLoc, &maxLoc, contour_mask);
            
            channel_keypoints.push_back(std::make_pair(maxLoc, max));
            //printf("Added keypoint %i %i %f\n", maxLoc.x, maxLoc.y, max);
        }
        
//         cv::Mat out;
//         cv::normalize(heatmap_channel, out, 0, 1, cv::NORM_MINMAX);                                                        
//         cv::imshow("nose",out);   
        // 
        return channel_keypoints;
    }
    
}


#endif
