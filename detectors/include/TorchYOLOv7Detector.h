/*
 * Attribute for importing and running YOLOv7 models (https://github.com/WongKinYiu/yolov7) with libtorch interface
 * Some processing stuff inspired by https://github.com/yasenh/libtorch-yolov5
 */

#ifndef _TORCH_YOLOV7_DETECTOR_H_
#define _TORCH_YOLOV7_DETECTOR_H_

#include "Attribute.h"
#include <torch/torch.h>
#include <torch/script.h>

namespace eod{
        
    enum YOLOV7_TYPE {BOX, POSE, SEG};
    
    at::Tensor tensor_from_mat(const cv::Mat& image);
    std::vector<float> LetterboxImage(const cv::Mat& src, cv::Mat& dst, const cv::Size& out_size);
    
    class TorchYOLOv7Attribute : public Attribute{
    public: 
        
        TorchYOLOv7Attribute(std::string model_path, int input_size, std::string lales_path);
        
        std::vector<ExtendedObjectInfo> Detect2(const InfoImage& image, int seq);
                
        bool Check2(const InfoImage& image, ExtendedObjectInfo& rect);
        
        void Extract2(const InfoImage& image, ExtendedObjectInfo& rect);
    private:
        int input_size_;
        YOLOV7_TYPE net_type;
        torch::jit::script::Module module;
        std::map<int, std::string> labelsMap;
        
    };
    
}

#endif // _TORCH_YOLOV7_DETECTOR_H_
