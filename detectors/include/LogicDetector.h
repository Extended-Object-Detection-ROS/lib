#ifndef _LOGIC_ATTRIBUTE_
#define _LOGIC_ATTRIBUTE_

#include "Attribute.h"

namespace eod{
    
    // ------------------------
    // AND
    // ------------------------
    class AndAttribute : public Attribute{
    public:
        
        AndAttribute();
        AndAttribute(Attribute*, Attribute*, double, bool second_check = false);
        
        std::vector<ExtendedObjectInfo> Detect2(const InfoImage& image, int seq);
        
        bool Check2(const InfoImage& image, ExtendedObjectInfo& rect);
        
        void Extract2(const InfoImage& image, ExtendedObjectInfo& rect);
        
    private:
        Attribute* attributeA;
        Attribute* attributeB;
        double iou_threshold;
        bool second_check;
        
    };
    
    // ------------------------
    // NOT
    // ------------------------
    enum NotAttributeMethod {NAM_CHECK, NAM_DETECT};
    
    class NotAttribute : public Attribute{
    public:
        
        NotAttribute();
        NotAttribute(Attribute*, NotAttributeMethod method = NotAttributeMethod::NAM_CHECK, double iou = 0.75);
        
        std::vector<ExtendedObjectInfo> Detect2(const InfoImage& image, int seq);
        
        bool Check2(const InfoImage& image, ExtendedObjectInfo& rect);
        
        void Extract2(const InfoImage& image, ExtendedObjectInfo& rect);
        
    private:
        Attribute* attribute;       
        
        NotAttributeMethod method;
        double iou;
    };
    
    
    // ------------------------
    // OR
    // ------------------------
    class OrAttribute : public Attribute{
    public:
        
        OrAttribute();
        OrAttribute(Attribute*, Attribute*, double);
        
        std::vector<ExtendedObjectInfo> Detect2(const InfoImage& image, int seq);
        
        bool Check2(const InfoImage& image, ExtendedObjectInfo& rect);
        
        void Extract2(const InfoImage& image, ExtendedObjectInfo& rect);
        
    private:
        Attribute* attributeA;
        Attribute* attributeB;
        double iou_threshold;    
    };
    
    
    // ------------------------
    // XOR
    // ------------------------
    
}

#endif 
