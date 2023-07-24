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
        
        //[19][2][2]
        const std::vector<std::vector<std::vector<int>>> paf_keys = {{{1, 2}, {12, 13}}, {{1, 5}, {20, 21}}, {{2, 3}, {14, 15}},
                 {{3, 4}, {16, 17}}, {{5, 6}, {22, 23}}, {{6, 7}, {24, 25}},
                 {{1, 8}, {0, 1}}, {{8, 9}, {2, 3}}, {{9, 10}, {4, 5}},
                 {{1, 11}, {6, 7}}, {{11, 12}, {8, 9}}, {{12, 13}, {10, 11}},
                 {{1, 0}, {28, 29}}, {{0, 14}, {30, 31}}, {{14, 16}, {34, 35}},
                 {{0, 15}, {32, 33}}, {{15, 17}, {36, 37}},
                 {{2, 17}, {18, 19}}, {{5, 16}, {26, 27}}};
                 
        int _num_paf_samples = 10;
        double _min_paf_score_th = 2;
        double _paf_sample_th = 4;
            
        
    };
    
}


#endif //_ROS_SUBSCRIBER_OPEN_POSE_RAW_
