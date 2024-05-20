#ifndef _DUMMY_HUMAN_POSE_ATTRIBUTE_
#define _DUMMY_HUMAN_POSE_ATTRIBUTE_

#include "Attribute.h"

namespace eod{
        

    class DummyHumanPoseAttribute : public Attribute{
    public:
        
        DummyHumanPoseAttribute();        
        
        DummyHumanPoseAttribute(int mode, double max_dist_m, int radius);
        
        std::vector<ExtendedObjectInfo> Detect2(const InfoImage& image, int seq);
        
        bool Check2(const InfoImage& image, ExtendedObjectInfo& rect);
        
        void Extract2(const InfoImage& image, ExtendedObjectInfo& rect);
        
    private:        
        
        bool isHandRaised(ExtendedObjectInfo& rect);
        
    };
    
}

#endif // _DUMMY_HUMAN_POSE_ATTRIBUTE_
