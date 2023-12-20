#ifndef _ROS_SUBSCRIBER_OPEN_POSE_RAW_
#define _ROS_SUBSCRIBER_OPEN_POSE_RAW_

#include "ROSSubscriberBaseDetector.h"
#include "depthai_ros_extended_msgs/NeuralNetworkRawOutput.h"

namespace eod{        
    
    class ROSSubscriberOpenPoseRaw : public ROSSubscriberBaseAttribute<depthai_ros_extended_msgs::NeuralNetworkRawOutput>{
    public:
        
        ROSSubscriberOpenPoseRaw(std::string topic_name, float timelag, float timeshift, float kpt_score_th = 0.15, int num_paf_samples = 10, float min_paf_score_th = 0.2, float paf_sample_th = 0.4);                
        
        void callback(const depthai_ros_extended_msgs::NeuralNetworkRawOutput::ConstPtr& msg);
        
        std::vector<ExtendedObjectInfo> Detect2(const InfoImage& image, int seq);
        
        void Extract2(const InfoImage& image, ExtendedObjectInfo& rect);
        
        bool Check2(const InfoImage& image,ExtendedObjectInfo& rect);                
                            
    private:
        
        std::vector<std::pair<cv::Point, float>> get_keypoints_single_channel(const cv::Mat& heatmap_channel, const cv::Size& original_size);
        
        const std::vector<std::string> landmarks_labels = {"nose", "neck", "right_shoulder", "right_elbow", "right_wrist",
        "left_shoulder", "left_elbow", "left_wrist", "right_hip", "right_knee",
        "right_ankle", "left_hip", "left_knee", "left_ankle", "right_eye",
        "left_eye", "right_ear", "left_ear"};
                
        const std::vector<std::vector<std::vector<int>>> paf_keys = {{{1, 2}, {12, 13}}, {{1, 5}, {20, 21}}, {{2, 3}, {14, 15}},
                 {{3, 4}, {16, 17}}, {{5, 6}, {22, 23}}, {{6, 7}, {24, 25}},
                 {{1, 8}, {0, 1}}, {{8, 9}, {2, 3}}, {{9, 10}, {4, 5}},
                 {{1, 11}, {6, 7}}, {{11, 12}, {8, 9}}, {{12, 13}, {10, 11}},
                 {{1, 0}, {28, 29}}, {{0, 14}, {30, 31}}, {{14, 16}, {34, 35}},
                 {{0, 15}, {32, 33}}, {{15, 17}, {36, 37}},
                 {{2, 17}, {18, 19}}, {{5, 16}, {26, 27}}};
                            
        int _num_paf_samples;
        float _min_paf_score_th;
        float _paf_sample_th;
        float _kpt_score_th;
                    
    };    
}


#endif //_ROS_SUBSCRIBER_OPEN_POSE_RAW_
