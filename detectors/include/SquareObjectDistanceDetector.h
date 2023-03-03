#ifndef _SQUARE_OBJECT_DISTANCE_DETECT_
#define _SQUARE_OBJECT_DISTANCE_DETECT_

#include "Attribute.h"

namespace eod{
    
    class SquareObjectDistanceAttribute : public Attribute{
        
    public:
        SquareObjectDistanceAttribute(double length);
        
        std::vector<ExtendedObjectInfo> Detect2(const InfoImage& image, int seq);
        
        bool Check2(const InfoImage& image,ExtendedObjectInfo& rect);
        
        void Extract2(const InfoImage& image, ExtendedObjectInfo& rect);
        
    private:
        
        double length_;
        
        
        
    };
    
}

#endif
