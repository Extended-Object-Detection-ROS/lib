#ifndef _DEPTH_ATTRIBUTE_
#define _DEPTH_ATTRIBUTE_

#include "Attribute.h"

namespace eod{
    
    enum DEPTH_EXTRACT_MODE {ALL_BOX, HALF_SIZE_BOX};

    class DepthAttribute : public Attribute{
    public:
        
        DepthAttribute();        
        
        DepthAttribute(double depth_scale, int mode );
        
        std::vector<ExtendedObjectInfo> Detect2(const InfoImage& image, int seq);
        
        bool Check2(const InfoImage& image, ExtendedObjectInfo& rect);
        
        void Extract2(const InfoImage& image, ExtendedObjectInfo& rect);
        
    private:
        double depth_scale;
        int mode;
        
    };
    
}
#endif 
