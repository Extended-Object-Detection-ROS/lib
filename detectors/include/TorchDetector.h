#ifndef _TORCH_DETECTOR_H_
#define _TORCH_DETECTOR_H_

#include "Attribute.h"
#include <torch/torch.h>
#include <torch/script.h>

namespace eod{
    
    class TorchAttribute : public Attribute{
    public: 
        
        TorchAttribute(std::string model_path, int input_size);
        
        std::vector<ExtendedObjectInfo> Detect2(const InfoImage& image, int seq);
                
        bool Check2(const InfoImage& image, ExtendedObjectInfo& rect);
        
        void Extract2(const InfoImage& image, ExtendedObjectInfo& rect);
    private:
        int input_size_;
        torch::jit::script::Module module;
        
    };
    
}

#endif // _TORCH_DETECTOR_H_
