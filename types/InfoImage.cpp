#include "InfoImage.h"

namespace eod{
    
    InfoImage::InfoImage() : cv::Mat() {}        
    
    InfoImage::InfoImage(cv::Mat image) : cv::Mat(image) {
        
    }
    
    //InfoImage::InfoImage(cv::Mat image, cv::Mat K__, cv::Mat D__, int seq, float timestamp, std::string frame_id) : cv::Mat(image) {
    InfoImage::InfoImage(cv::Mat image, cv::Mat K__, cv::Mat D__, int seq, int sec, int nsec, std::string frame_id) : cv::Mat(image) {
        K_ = cv::Mat(K__);
        D_ = cv::Mat(D__);
        seq_ = seq;
        //timestamp_ = timestamp;
        sec_ = sec;
        nsec_ = nsec;
        frame_id_ = frame_id;
    }
    
    InfoImage::~InfoImage(){
    }
    
}
