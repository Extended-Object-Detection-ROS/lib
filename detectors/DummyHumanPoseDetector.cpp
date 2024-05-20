#include "DummyHumanPoseDetector.h"
#include "geometry_utils.h"

using namespace std;
using namespace cv;

namespace eod{
    
    DummyHumanPoseAttribute::DummyHumanPoseAttribute(){
            
        Type = DUMMY_POSE_A;

    }
    
    vector<ExtendedObjectInfo> DummyHumanPoseAttribute::Detect2(const InfoImage& image, int seq){      
        vector<ExtendedObjectInfo> objects;    
        return objects;
    }
    
    bool DummyHumanPoseAttribute::Check2(const InfoImage& image, ExtendedObjectInfo& rect){
        return false;
    }
    
    void DummyHumanPoseAttribute::Extract2(const InfoImage& image, ExtendedObjectInfo& rect){
        
        if( isHandRaised(rect) )
            set_extracted_info(rect, "pose_detected", "hand_raised");
        else
            set_extracted_info(rect, "pose_detected", "unk");
        
    }
    
    bool DummyHumanPoseAttribute::isHandRaised(ExtendedObjectInfo& rect){
        
        
        int left_shoulder_id = rect.getKeypointByName("shoulder_left");
        int right_shoulder_id = rect.getKeypointByName("shoulder_right");
        int left_elbow_id = rect.getKeypointByName("elbow_left");
        int right_elbow_id = rect.getKeypointByName("elbow_right");
        int left_wrist_id = rect.getKeypointByName("wrist_left");
        int right_wrist_id = rect.getKeypointByName("wrist_right");
        int nose_id = rect.getKeypointByName("nose");
        
        int shoulder_distance = range(rect.keypoints[left_shoulder_id], rect.keypoints[right_shoulder_id]);
        int distance_threshold = shoulder_distance / 4;
        
        // left
        if( rect.keypoints[left_elbow_id].y < rect.keypoints[left_shoulder_id].y && rect.keypoints[left_wrist_id].y < rect.keypoints[left_elbow_id].y && rect.keypoints[left_wrist_id].x > rect.keypoints[nose_id].x - distance_threshold)
            return true;
        
        if( rect.keypoints[right_elbow_id].y < rect.keypoints[right_shoulder_id].y && rect.keypoints[right_wrist_id].y < rect.keypoints[right_elbow_id].y && rect.keypoints[right_wrist_id].x < rect.keypoints[nose_id].x + distance_threshold)
            return true;
        
        return false;
        
    }

}
