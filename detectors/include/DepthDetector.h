#ifndef _DEPTH_ATTRIBUTE_
#define _DEPTH_ATTRIBUTE_

#include "Attribute.h"

namespace eod{
    
    enum DEPTH_EXTRACT_MODE {ALL_BOX, HALF_SIZE_BOX, CENTER_PX, MASK};

    class DepthAttribute : public Attribute{
    public:
        
        DepthAttribute();        
        
        DepthAttribute(int mode, double max_dist_m );
        
        std::vector<ExtendedObjectInfo> Detect2(const InfoImage& image, int seq);
        
        bool Check2(const InfoImage& image, ExtendedObjectInfo& rect);
        
        void Extract2(const InfoImage& image, ExtendedObjectInfo& rect);
        
    private:        
        int mode;
        double max_dist_m;
    };
    
}
#endif 
