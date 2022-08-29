#ifndef TD_RANGE_RELATION_H
#define TD_RANGE_RELATION_H

#include "Relationship.h"

namespace eod{
    
    enum THREE_DIM_RANGE_REL_SUBTYPE{
        CLOSED_RANGE,
        PROB_RANGE,
    };
    
    class ThreeDimRangeRelation : public RelationShip{
    public:
        ThreeDimRangeRelation();
        ThreeDimRangeRelation(int mode, double distLow_, double distHigh_);
        
        bool checkRelation(const cv::Mat& image, ExtendedObjectInfo* A, ExtendedObjectInfo* B);
        
        double checkSoft(const cv::Mat& image, ExtendedObjectInfo* A, ExtendedObjectInfo* B);
        
        void extractParams(const cv::Mat& image, ExtendedObjectInfo* A, ExtendedObjectInfo* B);
        
    private:
        int sub_type;
        double dist_low, dist_high;
        double dist, prob;
        
    };
    
}

#endif //TD_RANGE_RELATION_H
