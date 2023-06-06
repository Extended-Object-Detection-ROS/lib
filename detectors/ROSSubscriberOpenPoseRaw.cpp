#if (USE_ROS)

#include "ROSSubscriberOpenPoseRaw.h"

namespace eod{
    
    ROSSubscriberOpenPoseRaw::ROSSubscriberOpenPoseRaw(std::string topic_name, float timelag) : ROSSubscriberBaseAttribute(topic_name, timelag){
        Type = ROS_SUB_OPENPOSE_RAW_A;        
    }
    
//     void ROSSubscriberOpenPoseRaw::Connect2ROS(ros::NodeHandle nh){
//         subscriber_ = new message_filters::Subscriber<depthai_ros_extended_msgs::NeuralNetworkRawOutput>(nh, topic_name_, 1); 
//         
//         cache_ = new message_filters::Cache<depthai_ros_extended_msgs::NeuralNetworkRawOutput>(*subscriber_, 100);
//         cache_->registerCallback(&ROSSubscriberOpenPoseRaw::callback, this);
//     }
    
//     void ROSSubscriberOpenPoseRaw::callback(const depthai_ros_extended_msgs::NeuralNetworkRawOutput::ConstPtr& msg){
//         printf("Got msg!\n");
//         last_msg = *msg;
//     }
    
    std::vector<ExtendedObjectInfo> ROSSubscriberOpenPoseRaw::Detect2(const InfoImage& image, int seq){
        std::vector<ExtendedObjectInfo> results;        
        
//         auto msgs = cache_->getInterval(ros::Time(image.timestamp()), ros::Time(image.timestamp() + timelag_));        
//         printf("Len is %i\n", msgs.size());        
//         for( const auto& msg : msgs ){
//             if( msg->header.frame_id == image.frame_id() ){                
//                 
//                 printf("Good msg\n");
//                 
//                 return results;                                
//             }
//             else{
//                 printf("Message was skipped due other frame_id\n");
//             }
//         }        
//         printf("No appropriate time msgs\n");
        
        //printf("latests %f, image %f\n", cache_->getLatestTime().toSec(), image.timestamp());
        
        auto msg = cache_->getElemAfterTime(ros::Time(image.timestamp()));
        if( msg == nullptr ){
            printf("No msg\n");
        }
        else{
            printf("yes msg\n");
        }
            
        
        return results;
    }
    
    void ROSSubscriberOpenPoseRaw::Extract2(const InfoImage& image, ExtendedObjectInfo& rect){
    }
    
    bool ROSSubscriberOpenPoseRaw::Check2(const InfoImage& image,ExtendedObjectInfo& rect){
        return false;
    }  
}


#endif
