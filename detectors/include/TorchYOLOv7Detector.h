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
    
    at::Tensor tensor_from_mat(const cv::Mat& image);
    std::vector<float> LetterboxImage(const cv::Mat& src, cv::Mat& dst, const cv::Size& out_size);
    
    
    /*
     * Interface for nets with regular box output with output shapes [batch, n_detection, (xc, yc, w, h, p, p1, p2, p3 ... , pN) ]
     */
    class TorchYOLOv7Attribute : public Attribute{
    public: 
        
        TorchYOLOv7Attribute();
        TorchYOLOv7Attribute(std::string model_path, int input_size, std::string lables_path, bool force_cuda = false);
        
        std::vector<ExtendedObjectInfo> Detect2(const InfoImage& image, int seq);                
        bool Check2(const InfoImage& image, ExtendedObjectInfo& rect);        
        void Extract2(const InfoImage& image, ExtendedObjectInfo& rect);
    protected:
        int input_size_;        
        torch::jit::script::Module module;
        std::map<int, std::string> labelsMap;
        
        bool force_cuda;
                
        
    };
    
    
    /* 
     * Interface for nets with keypoint output like [batch, n_detection, (x, y, w, h, p, p_c1, ...,  p_Nc, {x_kp, y_kp, p_kp}*n_kp )
     * 
     */
    class TorchYOLOv7KeypointAttribute : public TorchYOLOv7Attribute{
    public:
        
        TorchYOLOv7KeypointAttribute(std::string model_path, int input_size, std::vector<std::string> kpt_lables, int num_class, int num_points, bool force_cuda = false);

        
        std::vector<ExtendedObjectInfo> Detect2(const InfoImage& image, int seq);                
        bool Check2(const InfoImage& image, ExtendedObjectInfo& rect);        
        void Extract2(const InfoImage& image, ExtendedObjectInfo& rect);
        
    private:
        int num_class_, num_points_;
        std::vector<std::string> kpt_lables_;
    };
    
}

#endif // _TORCH_YOLOV7_DETECTOR_H_
