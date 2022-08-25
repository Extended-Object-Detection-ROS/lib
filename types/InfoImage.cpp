#include "InfoImage.h"

namespace eod{
    
    InfoImage::InfoImage() : cv::Mat() {}        
    
    InfoImage::InfoImage(cv::Mat image) : cv::Mat(image) {
        
    }
    
    InfoImage::InfoImage(cv::Mat image, cv::Mat K__, cv::Mat D__) : cv::Mat(image) {
        K_ = cv::Mat(K__);
        D_ = cv::Mat(D__);
    }
    
    InfoImage::~InfoImage(){
    }
    
}
