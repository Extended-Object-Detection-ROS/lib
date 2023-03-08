#if (USE_TORCH)
#include "TorchDetector.h"


using namespace std;
using namespace cv;

namespace eod{
    
    
    TorchAttribute::TorchAttribute(std::string model_path, int input_size){
        Type = TORCH_A;
        module = torch::jit::load(model_path);
        input_size_ = input_size;
    }
    
    vector<ExtendedObjectInfo> TorchAttribute::Detect2(const InfoImage& image, int seq){        
        std::vector<ExtendedObjectInfo> answer;
        
        Mat blob;
        resize(image, blob, Size(input_size_, input_size_), INTER_LINEAR);
        
        std::vector<torch::jit::IValue> inputs;
//         inputs.push_back(torch::ones({1, 3, 224, 224}));
        
        at::Tensor tensor = torch::from_blob(blob.data, {1, input_size_, input_size_, 3 }, at::kByte);
        tensor = tensor.toType(c10::kFloat).div(255);
        tensor = tensor.permute({0,3,1,2});
        inputs.push_back(tensor);
        
        printf("FORWARD!\n");
                        
        at::Tensor output = module.forward(inputs).toTensor();
        
        //print("Output sizes %li\n", output.sizes()[0]);
        
        return answer;        
    }
    
    
    bool TorchAttribute::Check2(const InfoImage& image,ExtendedObjectInfo& rect){
        return false;        
    }
    
    void TorchAttribute::Extract2(const InfoImage& image, ExtendedObjectInfo& rect){
    }
    
    
}
#endif // USE_TORCH
