#if (USE_ROS)

#include "ROSSubscriberOpenPoseRaw.h"
#include "geometry_utils.h"

// Almost all logic is inspired by https://github.com/kschlegel/OAK-HumanPoseEstimation/blob/main/poseestimators/openpose.py


namespace eod{
    
    cv::Mat get_mat_from_data(const std::vector<float> &data, int channel, int width, int height, int type = CV_32F){
        auto start = data.begin() + channel * height * width;
        auto end = data.begin() + (1+channel) * height * width;
                            
        //cv::Mat channel_mat(height, width, type, std::vector<float>(start, end).data() );        
        cv::Mat channel_mat(height, width, type);
        int i = 0;
        for(auto it = start; it != end; it++, i++){
            int h = i % height;
            int w = i - height * h;
            channel_mat.at<float>(i) = (float)*it;
        }                
        return channel_mat;        
    }            
            
    ROSSubscriberOpenPoseRaw::ROSSubscriberOpenPoseRaw(std::string topic_name, float timelag, float kpt_score_th, int num_paf_samples, float min_paf_score_th, float paf_sample_th) : ROSSubscriberBaseAttribute(topic_name, timelag){
        Type = ROS_SUB_OPENPOSE_RAW_A;        
        _num_paf_samples = num_paf_samples;
        _min_paf_score_th = min_paf_score_th;
        _paf_sample_th = paf_sample_th;
        _kpt_score_th = kpt_score_th;
    }
    
    std::vector<ExtendedObjectInfo> ROSSubscriberOpenPoseRaw::Detect2(const InfoImage& image, int seq){
        ros::Time now = ros::Time::now();
        ros::Time imtime = ros::Time(image.sec_, image.nsec_);
        std::vector<ExtendedObjectInfo> results;                            
        
        auto msg = cache_->getElemAfterTime(imtime);
        int num_keypoints = landmarks_labels.size();
        do{                        
            if( msg == nullptr ){                
                msg = cache_->getElemAfterTime(imtime);
            }
            
            else{     
                
                if( msg->header.frame_id != image.frame_id() ){
                    printf("Frame ids does not match (%s and %s)\n", msg->header.frame_id.c_str(), image.frame_id().c_str());
                    continue;
                }
                
                cv::Size original_size = cv::Size(msg->input_width, msg->input_height);
                
                // PROCESS DATA
                std::vector<std::vector<std::pair<cv::Point, float>>> keypoints; // channel vise keypoints
                cv::Size proc_size;
                // get keypoints
                for( const auto &layer : msg->layers){
                    if( layer.name == "heatmaps" ){
                        int width, height;
                        for( const auto& dim : layer.tensor.layout.dim ){
                            if( dim.label == "channel" )
                                if( num_keypoints > dim.size ){
                                    printf("Decoding error! Num rekypoint i sless than size of tensor\n");
                                    return results;
                                }
                            if( dim.label == "height" )
                                height = dim.size;
                            if( dim.label == "width" )
                                width = dim.size;
                        }
                        proc_size = cv::Size(width, height);
                        for( size_t i = 0 ; i < num_keypoints; i++){                        
                            
                            cv::Mat channel = get_mat_from_data(layer.tensor.data, i, width, height);                            
                            keypoints.push_back(get_keypoints_single_channel(channel, original_size));
                        }                        
                    }
                }                
                // get connections            
                std::vector<std::vector<std::pair<cv::Point, float>>> pairs;
                for( const auto &layer : msg->layers){
                    if( layer.name == "pafs" ){
                        int height, width;
                        for( const auto& dim : layer.tensor.layout.dim ){
                            if( dim.label == "height" )
                                height = dim.size;
                            if( dim.label == "width" )
                                width = dim.size;
                        }
                        
                        // this pairs grouped by connections, each member ikeypointss indexes of keypoits in theirs channels and score of connection                                                
                        for( const auto& connection_pafs_ids : paf_keys ){
                                                                                    
                            auto connection = connection_pafs_ids[0];
                            auto pafs_ids = connection_pafs_ids[1];
                            
                            //printf("Pair (%s-%s)\n", landmarks_labels[connection[0]].c_str(), landmarks_labels[connection[1]].c_str());                            
                            cv::Mat paf_a = get_mat_from_data(layer.tensor.data, pafs_ids[0], width, height, CV_32F);
                            cv::Mat paf_b = get_mat_from_data(layer.tensor.data, pafs_ids[1], width, height, CV_32F);                                                        
                            
                            cv::resize(paf_a, paf_a, original_size);
                            cv::resize(paf_b, paf_b, original_size);
                                                                                    
                            std::vector<std::pair<cv::Point, float>>* cand_a = &(keypoints[connection[0]]);
                            std::vector<std::pair<cv::Point, float>>* cand_b = &(keypoints[connection[1]]);
                                                        
                            std::vector<std::pair<cv::Point, float>> pairs_found;                                                        
                            for( size_t i = 0 ; i < cand_a->size() ; i++){
                                
                                int max_j = -1;
                                float max_score = -1;
                                
                                for( size_t j = 0 ; j < cand_b->size() ; j++){                                    
                                    
                                    float unit_a2b_x = cand_b->at(j).first.x - cand_a->at(i).first.x;
                                    float unit_a2b_y = cand_b->at(j).first.y - cand_a->at(i).first.y;
                                    
                                    float vec_norm = std::sqrt( unit_a2b_x * unit_a2b_x + unit_a2b_y * unit_a2b_y); 
                                                                                                            
                                    if( vec_norm){                          
                                        unit_a2b_x /= vec_norm;
                                        unit_a2b_y /= vec_norm;
                                    }
                                    else{
                                        continue;
                                    }
                                                                                                                               
                                    std::vector<std::pair<float, float>> paf_samples;                                                                        
                                    float x_len = cand_b->at(j).first.x - cand_a->at(i).first.x;
                                    float y_len = cand_b->at(j).first.y - cand_a->at(i).first.y;
                                                                        
                                    for( int k = 0 ; k < _num_paf_samples ; k++ ){
                                        float coord_x = cand_a->at(i).first.x + (x_len / (_num_paf_samples - 1)) * k;
                                        float coord_y = cand_a->at(i).first.y + (y_len / (_num_paf_samples - 1)) * k;
                                                               
                                        int x = int(std::round(coord_x));
                                        int y = int(std::round(coord_y));                                                                                

                                        paf_samples.push_back(std::make_pair(paf_a.at<float>(y, x), paf_b.at<float>(y, x)));                                        
                                    }                                                          
                                    
                                    std::vector<float> paf_scores;
                                    for( int k = 0 ; k < _num_paf_samples ; k++ ){
                                        float dot_prod = paf_samples[k].first * unit_a2b_x + paf_samples[k].second * unit_a2b_y;                                        
                                        paf_scores.push_back(dot_prod);                                        
                                    }
                                    
                                    int good_paf_scores = 0;
                                    float sum_scores = 0;
                                    for( int k = 0 ; k < _num_paf_samples; k++){
                                        if( paf_scores.at(k) > _min_paf_score_th)
                                            good_paf_scores++;
                                        sum_scores += paf_scores.at(k);
                                    }
                                    
                                    float mean_good_paf_score = float(good_paf_scores) / _num_paf_samples;
                                    if( mean_good_paf_score  > _paf_sample_th ){
                                        float avg_paf_score = sum_scores / paf_scores.size();
                                        if( avg_paf_score > max_score){
                                            max_j = j;
                                            max_score = avg_paf_score;
                                        }
                                    }                                                                                    
                                }                             
                                if( max_j >= 0 ){                                    
                                    pairs_found.push_back(std::make_pair(cv::Point(i, max_j), max_score));                                    
                                }
                            }
                            pairs.push_back(pairs_found);
                        }                        
                    }
                }
                                                                                        
                // get personwise                                      
                // each person is array, where index is landmark id, and value is its number in keypoints subbarray of same landmark id
                std::vector<std::vector<int>> persons;                
                
                for( size_t connection_id = 0 ; connection_id < paf_keys.size() ; connection_id++){
                    
                    if( pairs[connection_id].size() == 0 ){
                        //printf("Pair skipped (%s-%s)\n", landmarks_labels[paf_keys[connection_id][0][0]].c_str(), landmarks_labels[paf_keys[connection_id][0][1]].c_str());
                        continue;
                    }
                    //printf("Pair ok (%s-%s)\n", landmarks_labels[paf_keys[connection_id][0][0]].c_str(), landmarks_labels[paf_keys[connection_id][0][1]].c_str());
                    
                    int start_landmark_id = paf_keys[connection_id][0][0];
                    int end_landmark_id = paf_keys[connection_id][0][1];
                    
                    for( size_t i = 0 ; i < pairs[connection_id].size() ; i++ ){
                        // # of keypoint in keypoints array
                        int start_keypoint_id = pairs[connection_id][i].first.x;
                        int end_keypoint_id = pairs[connection_id][i].first.y;                        
                        
                        bool person_found = false;
                        for( size_t j = 0 ; j < persons.size() ; j++ ){                            
                            if( persons[j][start_landmark_id] == start_keypoint_id ){                                                                                                 
                                persons[j][end_landmark_id] = end_keypoint_id;                          
                                person_found = true;                                
                                break;
                            }                                
                        }
                        if( !person_found && connection_id < paf_keys.size() - 2){
                            std::vector<int> person(num_keypoints);
                            for( size_t j = 0 ; j < num_keypoints; j++)
                                person[j] = -1;
                            person[start_landmark_id] = start_keypoint_id;
                            person[end_landmark_id] = end_keypoint_id;                          
                            persons.push_back(person);
                        }
                    }                    
                }                

                for( auto& person : persons ){
                    ExtendedObjectInfo tmp(0, 0, 2, 2);
                    double conf = 0;                    
                    
                    for( size_t kpt_id = 0 ; kpt_id < person.size() ; kpt_id++ ){
                        if( person[kpt_id] != -1 ){                            
                            auto kpt = keypoints[kpt_id][person[kpt_id]];                            
                            tmp.keypoints.push_back(eod::KeyPoint(scale_point(kpt.first, original_size, image.size()), kpt.second, landmarks_labels[kpt_id]));                            
                            
                            conf += kpt.second;
                        }                        
                    }
                    tmp.updateRectFromKeypoints(0.2);
                    for( size_t connection = 0 ; connection < paf_keys.size()-2 ; connection++ ){
                        int id1 = paf_keys[connection][0][0];
                        int id2 = paf_keys[connection][0][1];
                        
                        if( person[id1] != -1 && person[id2] != -1 ){
                            int real_id1 = id1 - std::count(person.begin(), person.begin() + id1, -1);
                            int real_id2 = id2 - std::count(person.begin(), person.begin() + id2, -1);                            
                            tmp.keypoint_connection.push_back(std::make_pair(real_id1, real_id2));
                                                        
                        }
                    }                    
                    conf /= (2 * paf_keys.size());                    
                    tmp.setScoreWeight(conf, Weight);
                    results.push_back(tmp);
                    
                }                                
                
                /*
                // TEST FOR DRAW ALL KPT AND CONNECTIONS
                ExtendedObjectInfo tmp(0, 0, 2, 2);                
                
                for( size_t p = 0 ; p < pairs.size() ; p++ ){
                    for( auto& pair : pairs[p] ){
                        int a = pair.first.x;
                        int b = pair.first.y;
                        
                        int kpt_a_id, kpt_b_id;
                        kpt_a_id = paf_keys[p][0][0];
                        kpt_b_id = paf_keys[p][0][1];
                                                                        
                        auto kpt_a = keypoints[kpt_a_id][a];
                        auto kpt_b = keypoints[kpt_b_id][b];
                        
                        tmp.keypoints.push_back(eod::KeyPoint(scale_point(kpt_a.first, original_size, image.size()), kpt_a.second, ""));                                                    
                        tmp.keypoints.push_back(eod::KeyPoint(scale_point(kpt_b.first, original_size, image.size()), kpt_b.second, ""));
                        
                        tmp.keypoint_connection.push_back(std::make_pair(tmp.keypoints.size()-1, tmp.keypoints.size()-2));                        
                    }
                }
                // repeat kpts                 
                for( auto &kpts : keypoints ){
                    for( auto& kpt : kpts ){
                        tmp.keypoints.push_back(eod::KeyPoint(scale_point(kpt.first, original_size, image.size()), kpt.second, ""));                                                    
                    }
                }
                results.push_back(tmp);
                */                                                                
                return results;
            }                        
            
        }  
        while( (ros::Time::now() - now).toSec() < timelag_ && ros::ok());                
        return results;
    }        
    
    void ROSSubscriberOpenPoseRaw::Extract2(const InfoImage& image, ExtendedObjectInfo& rect){
    }
    
    bool ROSSubscriberOpenPoseRaw::Check2(const InfoImage& image,ExtendedObjectInfo& rect){
        return false;
    }  
    
    std::vector<std::pair<cv::Point, float>> ROSSubscriberOpenPoseRaw::get_keypoints_single_channel(const cv::Mat& heatmap_channel, const cv::Size& original_size){
        
        std::vector<std::pair<cv::Point,float> > channel_keypoints;
        
        cv::Mat resized;
        cv::resize(heatmap_channel, resized, original_size);
        cv::GaussianBlur(resized, resized, cv::Size(3,3), 0, 0);
        
        // filter by prob        
        cv::Mat mask;
        cv::inRange(resized, _kpt_score_th, 1.0, mask);
        
        // find clusters
        std::vector<std::vector<cv::Point> > contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours( mask, contours, hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE );
        
        // get cluster centers
        for( const auto& contour : contours ){
            cv::Mat contour_mask = cv::Mat::zeros(resized.size(), CV_8U);
            cv::fillConvexPoly(contour_mask, contour, 1);
            double max, min;
            cv::Point maxLoc, minLoc;
            cv::minMaxLoc(resized, &min, &max, &minLoc, &maxLoc, contour_mask);            
            channel_keypoints.push_back(std::make_pair(maxLoc, max));                
            //printf("Added keypoint %i %i %f\n", maxLoc.x, maxLoc.y, (float)max);
        }
        return channel_keypoints;
    }
}

#endif
