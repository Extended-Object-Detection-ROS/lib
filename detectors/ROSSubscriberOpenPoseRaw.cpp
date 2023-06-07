#if (USE_ROS)

#include "ROSSubscriberOpenPoseRaw.h"

namespace eod{
    
    ROSSubscriberOpenPoseRaw::ROSSubscriberOpenPoseRaw(std::string topic_name, float timelag) : ROSSubscriberBaseAttribute(topic_name, timelag){
        Type = ROS_SUB_OPENPOSE_RAW_A;        
    }
    
    std::vector<ExtendedObjectInfo> ROSSubscriberOpenPoseRaw::Detect2(const InfoImage& image, int seq){
        ros::Time now = ros::Time::now();
        std::vector<ExtendedObjectInfo> results;        
            
        auto msg = cache_->getElemAfterTime(ros::Time(image.timestamp()));
        while( (ros::Time::now() - now).toSec() < timelag_ && ros::ok()){            
            if( msg == nullptr ){
                //printf("waiting\n");
                msg = cache_->getElemAfterTime(ros::Time(image.timestamp()));
            }
            else{
                printf("yes msg\n");
                return results;
            }            
            printf("diff is %f\n", (ros::Time(image.timestamp()) - cache_->getLatestTime()).toSec() );
        }                        
        printf("no msg\n");
        return results;
    }
    
    void ROSSubscriberOpenPoseRaw::Extract2(const InfoImage& image, ExtendedObjectInfo& rect){
    }
    
    bool ROSSubscriberOpenPoseRaw::Check2(const InfoImage& image,ExtendedObjectInfo& rect){
        return false;
    }  
}


#endif
