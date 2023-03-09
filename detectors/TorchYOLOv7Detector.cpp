#if (USE_TORCH)
#include "TorchYOLOv7Detector.h"


using namespace std;
using namespace cv;

namespace eod{
    
    at::Tensor tensor_from_mat(const cv::Mat& image, int size){
        Mat blob;        
        cvtColor(image, blob, COLOR_BGR2RGB); 
        resize(blob, blob, Size(size, size), INTER_LINEAR);        
        at::Tensor tensor = torch::from_blob(blob.data, {1, size, size, 3 }, at::kByte);
        tensor = tensor.toType(c10::kFloat).div(255);
        tensor = tensor.permute({0,3,1,2});
        return tensor;
    }
    
    TorchYOLOv7Attribute::TorchYOLOv7Attribute(std::string model_path, int input_size){
        Type = TORCH_YOLOV7_A;
        net_type = BOX;
        module = torch::jit::load(model_path);
        input_size_ = input_size;
    }
    
    vector<ExtendedObjectInfo> TorchYOLOv7Attribute::Detect2(const InfoImage& image, int seq){        
        std::vector<ExtendedObjectInfo> answer;
        
        Mat blob;
        resize(image, blob, Size(input_size_, input_size_), INTER_LINEAR);
        
        std::vector<torch::jit::IValue> inputs;
        inputs.push_back(tensor_from_mat(image, input_size_));
        
        printf("FORWARD!\n");                    
        /*
         * output shape is [batch = 1, n_detections, 5 + n_classes]
         * for each detection comes [x, y, w, h, p_obj, p_class1, p_class2, ..., p_classN]
         */
        auto output = module.forward(inputs).toTuple()->elements()[0].toTensor();
        
        // postprocess
        int item_attr_size = 5;
        auto num_classes = output.size(2) - item_attr_size;
        // get candidates which object confidence > threshold
        auto conf_mask = output.select(2, 4).ge(Probability).unsqueeze(2);                
        
        auto det = torch::masked_select(output[0], conf_mask[0]).view({-1, num_classes + item_attr_size}); // allows only 1-batched output
        
        if (det.size(0) != 0){
            // compute overall score = obj_conf * cls_conf, similar to x[:, 5:] *= x[:, 4:5]
            det.slice(1, item_attr_size, item_attr_size + num_classes) *= det.select(1, 4).unsqueeze(1);
            
            // [best class only] get the max classes score at each result (e.g. elements 5-84)
            std::tuple<torch::Tensor, torch::Tensor> max_classes = torch::max(det.slice(1, item_attr_size, item_attr_size + num_classes), 1);

            // class score
            auto max_conf_score = std::get<0>(max_classes);
            // index
            auto max_conf_index = std::get<1>(max_classes);

            max_conf_score = max_conf_score.to(torch::kFloat).unsqueeze(1);
            max_conf_index = max_conf_index.to(torch::kFloat).unsqueeze(1);
            
            // shape: n * 6, x (0), y(1), w(2), h(3), score(4), class index(5)
            det = torch::cat({det.slice(1, 0, 4), max_conf_score, max_conf_index}, 1);
            
            const auto& det_array = det.accessor<float, 2>();
            for(int i = 0; i < det.size(0) ; i++){
                
                int x = int(det_array[i][0] * image.cols);
                int y = int(det_array[i][1] * image.rows);
                int w = int(det_array[i][2] * image.cols);
                int h = int(det_array[i][3] * image.rows);
                
                ExtendedObjectInfo tmp = ExtendedObjectInfo(Rect(x, y, w, h));
                tmp.setScoreWeight(det_array[i][4], Weight);
                set_extracted_info(tmp, "class_id", (int)det_array[i][5]); 
                                
                answer.push_back(tmp);
            }
                        
        }                
        return answer;
    }
    
    
    bool TorchYOLOv7Attribute::Check2(const InfoImage& image,ExtendedObjectInfo& rect){
        return false;        
    }
    
    void TorchYOLOv7Attribute::Extract2(const InfoImage& image, ExtendedObjectInfo& rect){
    }
    
    
}
#endif // USE_TORCH
