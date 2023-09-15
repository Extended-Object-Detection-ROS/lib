#ifndef _KEYPOINT_POSE_DETECTOR_
#define _KEYPOINT_POSE_DETECTOR_

#include "Attribute.h"
#include <map>

namespace eod{
        
    class KeypointPoseAttribute : public Attribute{
    public:
        
        KeypointPoseAttribute(std::map<std::string, cv::Point3f> object_points);
                        
        std::vector<ExtendedObjectInfo> Detect2(const InfoImage& image, int seq);
        
        bool Check2(const InfoImage& image, ExtendedObjectInfo& rect);
        
        void Extract2(const InfoImage& image, ExtendedObjectInfo& rect);
        
    private:       
        std::map<std::string, cv::Point3f> _object_points;
        
    };
    
}
#endif // _KEYPOINT_POSE_DETECTOR_
