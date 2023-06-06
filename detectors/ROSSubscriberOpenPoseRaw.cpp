#if (USE_ROS)

#include "ROSSubscriberOpenPoseRaw.h"

namespace eod{
    
    ROSSubscriberOpenPoseRaw::ROSSubscriberOpenPoseRaw(std::string topic_name, float timelag) : ROSSubscriberBaseAttribute(topic_name){
        Type = ROS_SUB_OPENPOSE_RAW_A;
        timelag_ = timelag;
    }
    
    void ROSSubscriberOpenPoseRaw::callback(const depthai_ros_extended_msgs::NeuralNetworkRawOutput::ConstPtr& msg){
        printf("Got msg!\n");
        last_msg = *msg;
    }
    
    std::vector<ExtendedObjectInfo> ROSSubscriberOpenPoseRaw::Detect2(const InfoImage& image, int seq){
        std::vector<ExtendedObjectInfo> results;
        
        depthai_ros_extended_msgs::NeuralNetworkRawOutput msg_copy = last_msg;
        float lag = fabs(msg_copy.header.stamp.toSec() - image.timestamp() );
        if( lag > timelag_){
            printf("Message was skipped due old frame (lag = %.3f)\n", lag);
            return results;
        }
        if( msg_copy.header.frame_id != last_msg.header.frame_id ){
            printf("Message was skipped due other frame_id (%s)\n", msg_copy.header.frame_id.c_str());
            return results;
        }
        printf("Got msg\n");
        
        return results;
    }
    
    void ROSSubscriberOpenPoseRaw::Extract2(const InfoImage& image, ExtendedObjectInfo& rect){
    }
    
    bool ROSSubscriberOpenPoseRaw::Check2(const InfoImage& image,ExtendedObjectInfo& rect){
        return false;
    }  
}


#endif
