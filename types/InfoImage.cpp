#include "InfoImage.h"

namespace eod{
    
    InfoImage::InfoImage() : cv::Mat() {}        
    
    InfoImage::InfoImage(cv::Mat image) : cv::Mat(image) {
        
    }
    
    InfoImage::InfoImage(cv::Mat image, cv::Mat K__, cv::Mat D__, int seq) : cv::Mat(image) {
        K_ = cv::Mat(K__);
        D_ = cv::Mat(D__);
        seq_ = seq;
    }
    
    InfoImage::~InfoImage(){
    }
    
}
