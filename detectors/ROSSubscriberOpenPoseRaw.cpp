#if (USE_ROS)

#include "ROSSubscriberOpenPoseRaw.h"
#include "geometry_utils.h"

namespace eod{
    
    std::vector<cv::Point2f> get_keypoints_single_channel(const cv::Mat& heatmap_channel){
        
    }
    
    
    ROSSubscriberOpenPoseRaw::ROSSubscriberOpenPoseRaw(std::string topic_name, float timelag) : ROSSubscriberBaseAttribute(topic_name, timelag){
        Type = ROS_SUB_OPENPOSE_RAW_A;        
    }
    
    std::vector<ExtendedObjectInfo> ROSSubscriberOpenPoseRaw::Detect2(const InfoImage& image, int seq){
        ros::Time now = ros::Time::now();
        ros::Time imtime = ros::Time(image.sec_, image.nsec_);
        std::vector<ExtendedObjectInfo> results;        
            
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
                        for( size_t i = 0 ; i < 1; i++){//num_keypoints ; i++ ){
                            auto start = layer.tensor.data.begin() + i * height * width;
                            auto end = layer.tensor.data.begin() + (1+i) * height * width;
                            cv::Mat channel(height, width, CV_64F, std::vector<double>(start, end).data() );
                            cv::Mat out;
                            cv::normalize(channel, out, 0, 1, cv::NORM_MINMAX);
                            
                            //channel = normalize_float_mat(channel);
                            cv::imshow("nose",out);
                            //cv::waitKey();
                        }
                        
                    }
                }                
                return results;
            }                        
        }  
        while( (ros::Time::now() - now).toSec() < timelag_ && ros::ok());
        
        printf("no msg\n");
        printf("diff is %f\n", (imtime - cache_->getLatestTime()).toSec() );
        return results;
    }
    
    void ROSSubscriberOpenPoseRaw::Extract2(const InfoImage& image, ExtendedObjectInfo& rect){
    }
    
    bool ROSSubscriberOpenPoseRaw::Check2(const InfoImage& image,ExtendedObjectInfo& rect){
        return false;
    }  
    
    
}


#endif
