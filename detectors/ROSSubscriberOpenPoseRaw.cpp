#if (USE_ROS)

#include "ROSSubscriberOpenPoseRaw.h"
#include "geometry_utils.h"

namespace eod{
    
        
    ROSSubscriberOpenPoseRaw::ROSSubscriberOpenPoseRaw(std::string topic_name, float timelag) : ROSSubscriberBaseAttribute(topic_name, timelag){
        Type = ROS_SUB_OPENPOSE_RAW_A;        
    }
    
    std::vector<ExtendedObjectInfo> ROSSubscriberOpenPoseRaw::Detect2(const InfoImage& image, int seq){
        ros::Time now = ros::Time::now();
        ros::Time imtime = ros::Time(image.sec_, image.nsec_);
        std::vector<ExtendedObjectInfo> results;        
            
        ExtendedObjectInfo tmp;
        
        auto msg = cache_->getElemAfterTime(imtime);
        do{                        
            if( msg == nullptr ){
                //printf("waiting\n");
                msg = cache_->getElemAfterTime(imtime);
            }
            else{
                //printf("yes msg\n");
                //printf("diff is %f\n", (imtime - cache_->getLatestTime()).toSec() );
                
                // PROCESS DATA
                
                // get keypoints
                for( const auto &layer : msg->layers){
                    if( layer.name == "heatmaps" ){
                        int num_keypoints, width, height;
                        for( const auto& dim : layer.tensor.layout.dim ){
                            if( dim.label == "channel" )
                                num_keypoints = dim.size;
                            if( dim.label == "height" )
                                height = dim.size;
                            if( dim.label == "width" )
                                width = dim.size;
                        }
                        cv::Size proc_size(width, height);
                        for( size_t i = 0 ; i < num_keypoints; i++){
                            
                            auto start = layer.tensor.data.begin() + i * height * width;
                            auto end = layer.tensor.data.begin() + (1+i) * height * width;
                            
                            cv::Mat channel(height, width, CV_64F, std::vector<double>(start, end).data() );
                            
                            std::vector<std::pair<cv::Point, double>> keypoints = get_keypoints_single_channel(channel);
                            
                            //cv::Mat out;
                            //cv::normalize(channel, out, 0, 1, cv::NORM_MINMAX);                                                        
                            //cv::resize(out, out, cv::Size(msg->input_width, msg->input_height) );
                            
                            for( const auto& kpt : keypoints){
                                std::string label = landmarks_labels.size() > i ? landmarks_labels[i] : ""; 
                                tmp.keypoints.push_back(eod::KeyPoint(scale_point(kpt.first, proc_size, image.size()), kpt.second, label)); //DANGER better is too check size
                            }
                        }
                        
                    }
                }       
                tmp.setScoreWeight(1,1);
                results.push_back(tmp);
                return results;
            }                        
        }  
        while( (ros::Time::now() - now).toSec() < timelag_ && ros::ok());
        
        //printf("no msg\n");
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
        
        cv::Mat out;
        cv::normalize(heatmap_channel, out, 0, 1, cv::NORM_MINMAX);                                                        
        cv::imshow("nose",out);   
        // 
        return channel_keypoints;
    }
    
}


#endif
