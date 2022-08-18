#include "InfoImage.h"

namespace eod{
    
    InfoImage::InfoImage() : cv::Mat() {}
    
    
    InfoImage::InfoImage(cv::Mat image, cv::Mat K_, cv::Mat D_) : cv::Mat(image) {
        K = cv::Mat(K_);
        D = cv::Mat(D_);
    }
    
}
