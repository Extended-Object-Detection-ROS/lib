#ifndef _ROS_SUBSCRIBER_OPEN_POSE_RAW_
#define _ROS_SUBSCRIBER_OPEN_POSE_RAW_

#include "ROSSubscriberBaseDetector.h"
#include "depthai_ros_extended_msgs/NeuralNetworkRawOutput.h"

namespace eod{
    
    class ROSSubscriberOpenPoseRaw : public ROSSubscriberBaseAttribute<depthai_ros_extended_msgs::NeuralNetworkRawOutput>{
    public:
        
        ROSSubscriberOpenPoseRaw(std::string topic_name, float timelag);
        
        void callback(const depthai_ros_extended_msgs::NeuralNetworkRawOutput::ConstPtr& msg);
        
        std::vector<ExtendedObjectInfo> Detect2(const InfoImage& image, int seq);
        
        void Extract2(const InfoImage& image, ExtendedObjectInfo& rect);
        
        bool Check2(const InfoImage& image,ExtendedObjectInfo& rect);                
                            
    private:
        
        std::vector<std::pair<cv::Point, double>> get_keypoints_single_channel(const cv::Mat& heatmap_channel);
        
        const std::vector<std::string> landmarks_labels = {"nose", "neck", "right shoulder", "right elbow", "right wrist",
        "left shoulder", "left elbow", "left wrist", "right hip", "right knee",
        "right ankle", "left hip", "left knee", "left ankle", "right eye",
        "left eye", "right ear", "left ear"};
            
        
    };
    
}


#endif //_ROS_SUBSCRIBER_OPEN_POSE_RAW_
