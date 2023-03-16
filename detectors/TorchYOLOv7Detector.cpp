#if (USE_TORCH)
#include "TorchYOLOv7Detector.h"
#include <fstream> 

using namespace std;
using namespace cv;

namespace eod{
    
    bool readLabelsMapTxt(string filename, map<int, string> &labelsMap){
        std::ifstream file(filename);
        if (file.is_open()) {
            std::string str; 
            int counter = 0;
            while (std::getline(file, str)){
                labelsMap[counter] = str;
                counter++;
            }
            file.close();
            return true;
        }
        return false;
    }
    
    at::Tensor tensor_from_mat(const cv::Mat& image){
        Mat blob;        
        cvtColor(image, blob, COLOR_BGR2RGB); 
        //blob.convertTo(blob, CV_32FC3, 1.0f / 255.0f);
        //resize(blob, blob, Size(size, size), INTER_LINEAR);        
        at::Tensor tensor = torch::from_blob(blob.data, {1, image.rows, image.cols, 3 }, at::kByte);
        tensor = tensor.toType(c10::kFloat).div(255);
        tensor = tensor.permute({0,3,1,2}).contiguous();        
        return tensor;
    }
    
    std::vector<float> LetterboxImage(const cv::Mat& src, cv::Mat& dst, const cv::Size& out_size) {
        auto in_h = static_cast<float>(src.rows);
        auto in_w = static_cast<float>(src.cols);
        float out_h = out_size.height;
        float out_w = out_size.width;

        float scale = std::min(out_w / in_w, out_h / in_h);

        int mid_h = static_cast<int>(in_h * scale);
        int mid_w = static_cast<int>(in_w * scale);

        cv::resize(src, dst, cv::Size(mid_w, mid_h));

        int top = (static_cast<int>(out_h) - mid_h) / 2;
        int down = (static_cast<int>(out_h)- mid_h + 1) / 2;
        int left = (static_cast<int>(out_w)- mid_w) / 2;
        int right = (static_cast<int>(out_w)- mid_w + 1) / 2;

        cv::copyMakeBorder(dst, dst, top, down, left, right, cv::BORDER_CONSTANT, cv::Scalar(114, 114, 114));

        std::vector<float> pad_info{static_cast<float>(left), static_cast<float>(top), scale};
        return pad_info;
    }
    
    TorchYOLOv7Attribute::TorchYOLOv7Attribute(std::string model_path, int input_size, std::string lales_path){
        Type = TORCH_YOLOV7_A;
        net_type = BOX;
        module = torch::jit::load(model_path);
        input_size_ = input_size;
        readLabelsMapTxt(lales_path, labelsMap);
    }
    
    vector<ExtendedObjectInfo> TorchYOLOv7Attribute::Detect2(const InfoImage& image, int seq){        
        torch::NoGradGuard no_grad;
        
        std::vector<ExtendedObjectInfo> answer;                
        
        std::vector<torch::jit::IValue> inputs;
        Mat letter_box_image;
        std::vector<float> pad_info = LetterboxImage(image, letter_box_image, cv::Size(input_size_, input_size_));
        //printf("Padding w %f padding h %f scale %f\n",pad_info[0], pad_info[1], pad_info[2]);
        //imshow("letterbox", letter_box_image);
        
        inputs.push_back(tensor_from_mat(letter_box_image));
        
        //printf("FORWARD!\n");                    
        /*
         * output shape is [batch = 1, n_detections, 5 + n_classes]
         * for each detection comes [x_center, y_center, w, h, p_obj, p_class1, p_class2, ..., p_classN]
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
            auto det_cpu = det.cpu();
            
            const auto& det_array = det_cpu.accessor<float, 2>();
            for(int i = 0; i < det.size(0) ; i++){
                //printf("Result is %f %f %f %f\n", det_array[i][0], det_array[i][1], det_array[i][2], det_array[i][3]);
//                 int x = int(det_array[i][0] /input_size_ * image.cols);
//                 int y = int(det_array[i][1] /input_size_* image.rows);
//                 int x1 = int(det_array[i][2] /input_size_* image.cols);
//                 int y1 = int(det_array[i][3] /input_size_* image.rows);
                
                ExtendedObjectInfo not_scaled(Rect(int(det_array[i][0] - det_array[i][2]/2), 
                                                   int(det_array[i][1] -det_array[i][3]/2), 
                                                    int(det_array[i][2]),
                                                    int(det_array[i][3])));
                //ExtendedObjectInfo not_scaled(Rect(int(det_array[i][0]), int(det_array[i][1]), det_array[i][2],det_array[i][3]));
                not_scaled.draw(letter_box_image, Scalar(0,0,255));
                
                int xc = (det_array[i][0] - pad_info[0])/pad_info[2];
                int yc = (det_array[i][1] - pad_info[1])/pad_info[2];
                int w = (det_array[i][2])/pad_info[2];
                int h = (det_array[i][3])/pad_info[2];
                
                int x = xc - w/2;
                int y = yc - h/2;
                
                ExtendedObjectInfo tmp = ExtendedObjectInfo(Rect(x,y,w,h));
                tmp.setScoreWeight(det_array[i][4], Weight);
                int class_id = (int)det_array[i][5];
                set_extracted_info(tmp, "class_id", class_id); 
                
                if( labelsMap.size() ){
                    if( labelsMap.find(class_id) != labelsMap.end() ){
                        set_extracted_info(tmp, "class_label", labelsMap[class_id]);
                    }
                    else{
                        set_extracted_info(tmp, "class_label", "unknown");
                    }
                }
                                
                answer.push_back(tmp);
            }
            //imshow("letterbox_results", letter_box_image);
                        
        }
//         else{
//             printf("Nothing to detect!\n");
//         }
        return answer;
    }
    
    
    bool TorchYOLOv7Attribute::Check2(const InfoImage& image,ExtendedObjectInfo& rect){
        return false;        
    }
    
    void TorchYOLOv7Attribute::Extract2(const InfoImage& image, ExtendedObjectInfo& rect){
    }
    
    
}
#endif // USE_TORCH
