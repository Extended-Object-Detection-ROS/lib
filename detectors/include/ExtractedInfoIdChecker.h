/*
Project: Extended Object Detection Library
Author: Moscowsky Anton
File: Header file describes class for ExtractedInfo int values checker
*/

#ifndef _EXTRACTED_INFO_ID_CHECKER_
#define _EXTRACTED_INFO_ID_CHECKER_

#include "ObjectIdentifier.h"

namespace eod{
    
    class ExtractedInfoIdChecker : public Attribute{
    public:
        ExtractedInfoIdChecker();
        
        ExtractedInfoIdChecker(std::string field_, std::vector<int> allowed_, std::vector<int> forbidden_);
        
        std::vector<ExtendedObjectInfo> Detect2(const cv::Mat& image, int seq);
        bool Check2(const cv::Mat& image, ExtendedObjectInfo &rect);    
        void Extract2(const cv::Mat& image, ExtendedObjectInfo& rect);

    private:
        std::string field;
        std::vector<int> allowed;
        std::vector<int> forbidden;
        
    };
    
}

#endif // _EXTRACTED_INFO_ID_CHECKER_
