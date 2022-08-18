#ifndef _INFO_IMAGE_
#define _INFO_IMAGE_

#include <opencv2/opencv.hpp>

/*
 * Class inherited from cv::Mat but also containing K and D matrixes
 */

namespace eod{
    
    class InfoImage : public cv::Mat {
    public:
        InfoImage();
        InfoImage(cv::Mat image, cv::Mat K, cv::Mat D);
        
        cv::Mat K;
        cv::Mat D;
        
    private:
        
        
    };
    
}


#endif // _INFO_IMAGE_
