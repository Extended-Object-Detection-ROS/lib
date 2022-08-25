/*
Project: Extended Object Detection Library
Author: Moscowsky Anton
Atrribute for extracting unit translation to object
*/

#ifndef _UNIT_TRANSLATION_EXTRACTOR_
#define _UNIT_TRANSLATION_EXTRACTOR_

#include "ObjectIdentifier.h"

namespace eod{
    
    class UnitTranslationExtracter : public Attribute{
    public:
        UnitTranslationExtracter();
        
        std::vector<ExtendedObjectInfo> Detect2(const InfoImage& image, int seq);
        bool Check2(const InfoImage& image, ExtendedObjectInfo &rect);    
        void Extract2(const InfoImage& image, ExtendedObjectInfo& rect);

    private:        
    };    
}

#endif // _UNIT_TRANSLATION_EXTRACTOR_
