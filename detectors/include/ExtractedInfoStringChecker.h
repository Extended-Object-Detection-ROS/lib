/*
Project: Extended Object Detection Library
Author: Moscowsky Anton
File: Header file describes class for ExtractedInfo string values checker
*/

#ifndef _EXTRACTED_INFO_STRING_CHECKER_
#define _EXTRACTED_INFO_STRING_CHECKER_

#include "ObjectIdentifier.h"

namespace eod{
    
    class ExtractedInfoStringChecker : public Attribute{
    public:
        ExtractedInfoStringChecker();
        
        ExtractedInfoStringChecker(std::string field_, std::vector<std::string> allowed_, bool partially = false);
        
        std::vector<ExtendedObjectInfo> Detect2(const cv::Mat& image, int seq);
        bool Check2(const cv::Mat& image, ExtendedObjectInfo &rect);    
        void Extract2(const cv::Mat& image, ExtendedObjectInfo& rect);

    private:
        std::string field;
        std::vector<std::string> allowed;
        bool partially;
        
    };
    
}

#endif // _EXTRACTED_INFO_STRING_CHECKER_
