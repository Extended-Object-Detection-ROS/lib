#ifndef _DNN_DETECTOR_H_
#define _DNN_DETECTOR_H_

#include "Attribute.h"
#include <opencv2/dnn.hpp>

namespace eod{       
    
    enum DNN_FRMWRK{
        TF_DNN_FW,// tensorflow
        DN_DNN_FW,// darknet
    };
    
    class DnnAttribute : public Attribute{
    public:	    
        /// <summary>
        /// Default constructor
        /// </summary>
        DnnAttribute();
        
        DnnAttribute(std::string framework_name, std::string weights_file, std::string config_file, int inpWidth, int inpHeight, std::string labelMap, bool forceCuda =false);
        
        /// <summary>
        /// Detects aruco markers with 
        /// </summary>
        /// <param name="image">Destination image</param>
        /// <returns>Vector of rects of found objects</returns>
        std::vector<ExtendedObjectInfo> Detect2(const cv::Mat& image, int seq);
        
        /// <summary>
        /// 
        /// </summary>
        /// <param name="image">Destination image</param>
        /// <returns>Vector of rects of appropriate objects</returns>        
        bool Check2(const cv::Mat& image, ExtendedObjectInfo& rect);
        
        void Extract2(const cv::Mat& image, ExtendedObjectInfo& rect);
        
        std::string framework_name;
        std::string weights_file;
        std::string config_file;
        std::string label_file;
        int inpWidth;
        int inpHeight;
        
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
