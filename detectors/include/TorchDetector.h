#ifndef _TORCH_DETECTOR_H_
#define _TORCH_DETECTOR_H_

#include "Attribute.h"
#include <torch/torch.h>

namespace eod{
    
    class TorchAttribute : public Attribute{
    public: 
        
        TorchAttribute();
        
        std::vector<ExtendedObjectInfo> Detect2(const InfoImage& image, int seq);
                
        bool Check2(const InfoImage& image, ExtendedObjectInfo& rect);
        
        void Extract2(const InfoImage& image, ExtendedObjectInfo& rect);
    private:
        
    };
    
}

#endif // _TORCH_DETECTOR_H_
