#ifndef _DNN_DETECTOR_H_
#define _DNN_DETECTOR_H_

#include "Attribute.h"
#include <opencv2/dnn.hpp>

namespace eod{       
    
    enum DNN_FRMWRK{
        TF_DNN_FW, // tensorflow
        DN_DNN_FW, // darknet
        ONNX_DNN_FW, // ONNX is the best way to export from torch?
    };
    
    class DnnAttribute : public Attribute{
    public:	    
        /// <summary>
        /// Default constructor
        /// </summary>
        DnnAttribute();
        
        DnnAttribute(std::string framework_name, std::string weights_file, std::string config_file, int inpWidth, int inpHeight, std::string labelMap, bool forceCuda =false, std::vector<std::string> additional_layers = std::vector<std::string>(0), double maskProbability = 0.75);
        
        /// <summary>
        /// Detects aruco markers with 
        /// </summary>
        /// <param name="image">Destination image</param>
        /// <returns>Vector of rects of found objects</returns>
        std::vector<ExtendedObjectInfo> Detect2(const InfoImage& image, int seq);
        
        /// <summary>
        /// 
        /// </summary>
        /// <param name="image">Destination image</param>
        /// <returns>Vector of rects of appropriate objects</returns>        
        bool Check2(const InfoImage& image, ExtendedObjectInfo& rect);
        
        void Extract2(const InfoImage& image, ExtendedObjectInfo& rect);
        
        std::string framework_name;
        std::string weights_file;
        std::string config_file;
        std::string label_file;
        int inpWidth;
        int inpHeight;
        std::vector<std::string> process_layers_names;
        std::vector<std::string> process_layers_types;
        double maskProbability;
        
    private:        
        
        DNN_FRMWRK framework;
        cv::dnn::Net net;
        
        std::vector<ExtendedObjectInfo> saved_answer;
        int prev_seq;
        std::map<int, std::string> labelMap;
        bool isLabelMap;
        
    };
    
    
}
#endif //_DNN_DETECTOR_H_
