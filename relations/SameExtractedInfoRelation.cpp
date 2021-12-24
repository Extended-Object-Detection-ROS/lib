#include "SameExtractedInfoRelation.h"

namespace eod{
    SameExtractedInfoRelation::SameExtractedInfoRelation(){
        inited = false;
        Type = SAME_EXTR_INFO_R;
    }
    
    SameExtractedInfoRelation::SameExtractedInfoRelation(std::string key){
        inited = true;
        Type = SAME_EXTR_INFO_R;
        this->key = key;
    }
    
    bool SameExtractedInfoRelation::checkRelation(const cv::Mat& image, ExtendedObjectInfo* A, ExtendedObjectInfo* B){        
        if( A->extracted_info.count(key) && B->extracted_info.count(key) ){
            return A->extracted_info[key] == B->extracted_info[key];                
        }        
        return false;
    }   
    
}
