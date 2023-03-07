#if (USE_TORCH)
#include "TorchDetector.h"
using namespace std;
using namespace cv;

namespace eod{
    
    
    TorchAttribute::TorchAttribute(){
        torch::Tensor tensor = torch::rand({2, 3});
    }
    
    vector<ExtendedObjectInfo> TorchAttribute::Detect2(const InfoImage& image, int seq){
        
        std::vector<ExtendedObjectInfo> answer;
        return answer;        
    }
    
    
    bool TorchAttribute::Check2(const InfoImage& image,ExtendedObjectInfo& rect){
        return false;        
    }
    
    void TorchAttribute::Extract2(const InfoImage& image, ExtendedObjectInfo& rect){
    }
    
    
}
#endif // USE_TORCH
