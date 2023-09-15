#include "KeypointPoseDetector.h"


namespace eod{


    KeypointPoseAttribute::KeypointPoseAttribute(std::map<std::string, cv::Point3f> object_points){
        inited = true;
        Type = KPT_POSE_A;    
        _object_points = object_points;
    }
    
    std::vector<ExtendedObjectInfo> KeypointPoseAttribute::Detect2(const InfoImage& image, int seq){      
        std::vector<ExtendedObjectInfo> objects;    
        return objects;
    }
    
    bool KeypointPoseAttribute::Check2(const InfoImage& image, ExtendedObjectInfo& rect){
        return false;
    }
    
    void KeypointPoseAttribute::Extract2(const InfoImage& image, ExtendedObjectInfo& rect){
        
        std::vector<cv::Point3f> object_points;
        std::vector<cv::Point2f> image_points;
        
        for( const auto& kpt : rect.keypoints ){
            std::map<std::string, cv::Point3f>::iterator it = _object_points.find(kpt.label);
            if( it != _object_points.end() ){
                object_points.push_back(it->second);
                image_points.push_back(kpt);
            }
            
        }
        if( object_points.size() >= 4 ){
            cv::Vec3d rvec, tvec;
            cv::solvePnP(object_points, image_points, image.K(), image.D(), rvec, tvec, false, cv::SOLVEPNP_EPNP);
            rect.tvec.push_back(tvec);
            rect.rvec.push_back(rvec);
        }
        else{
            //printf("KeypointPoseAttribute: not enought points for PnP calculation\n");
            // destroy? but how
        }
        
    }
    
}
