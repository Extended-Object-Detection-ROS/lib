#ifndef _INFO_IMAGE_
#define _INFO_IMAGE_

#include <opencv2/opencv.hpp>

/*
 * Class inherited from cv::Mat but also containing K and D matrixes
 */

namespace eod{
        
    class polyMat : public cv::Mat{        
    public:
        polyMat() : cv::Mat(){}    
        polyMat(const cv::Mat& m) : cv::Mat(m){}
        
        virtual void foo() {}
        
    private:
        
    };
    
    class InfoImage : public polyMat {
    public:
        InfoImage();
        InfoImage(cv::Mat image, cv::Mat K, cv::Mat D);
        
        cv::Mat K;
        cv::Mat D;
        
        virtual ~InfoImage();
        
    private:
        
        
    };
    
}


#endif // _INFO_IMAGE_
