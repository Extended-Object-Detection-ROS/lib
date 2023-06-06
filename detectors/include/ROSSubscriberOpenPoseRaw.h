#ifndef _ROS_SUBSCRIBER_OPEN_POSE_RAW_
#define _ROS_SUBSCRIBER_OPEN_POSE_RAW_

#include "ROSSubscriberBaseDetector.h"
#include "depthai_ros_extended_msgs/NeuralNetworkRawOutput.h"

namespace eod{
    
    class ROSSubscriberOpenPoseRaw : public ROSSubscriberBaseAttribute{
    public:
        
        ROSSubscriberOpenPoseRaw(std::string topic_name, float timelag);
        
        void callback(const depthai_ros_extended_msgs::NeuralNetworkRawOutput::ConstPtr& msg);
        
        std::vector<ExtendedObjectInfo> Detect2(const InfoImage& image, int seq);
        
        void Extract2(const InfoImage& image, ExtendedObjectInfo& rect);
        
        bool Check2(const InfoImage& image,ExtendedObjectInfo& rect);
                            
    private:
        float timelag_;
    
        depthai_ros_extended_msgs::NeuralNetworkRawOutput last_msg;
        
    };
    
}


#endif //_ROS_SUBSCRIBER_OPEN_POSE_RAW_
