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
        InfoImage(cv::Mat image);
        InfoImage(cv::Mat image, cv::Mat K, cv::Mat D, int seq = 0);
                
        const cv::Mat& K() const {return K_;}
        const cv::Mat& D() const {return D_;}
        const int seq() const {return seq_;}
        
        void set_seq(int seq){
            seq_ = seq;
        }
        
        virtual ~InfoImage();
                        
    private:
        
        cv::Mat K_;
        cv::Mat D_;   
        
        int seq_;
    };
    
    
    
}


#endif // _INFO_IMAGE_
