#include "InfoImage.h"

namespace eod{
    
    InfoImage::InfoImage() : polyMat() {}
    
    InfoImage::~InfoImage(){
    }
    
    InfoImage::InfoImage(cv::Mat image, cv::Mat K_, cv::Mat D_) : polyMat(image) {
        K = cv::Mat(K_);
        D = cv::Mat(D_);
    }
    
}
