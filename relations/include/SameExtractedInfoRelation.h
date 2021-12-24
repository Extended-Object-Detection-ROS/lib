#ifndef SAME_EXTRACTED_INFO_RELATION_H
#define SAME_EXTRACTED_INFO_RELATION_H

#include "Relationship.h"

namespace eod{
    
    class SameExtractedInfoRelation : public RelationShip{
    public:
        SameExtractedInfoRelation();
        SameExtractedInfoRelation(std::string key);
        
        bool checkRelation(const cv::Mat& image, ExtendedObjectInfo* A, ExtendedObjectInfo* B);

    private:
        std::string key;
    };
    
}

#endif //SAME_EXTRACTED_INFO_RELATION_H
