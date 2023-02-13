#ifndef _CONTOUR_UTILS_
#define _CONTOUR_UTILS_

#include <opencv2/opencv.hpp>
#include <vector>

namespace eod{
    
    std::vector< cv::Point> shift_contour(std::vector <cv::Point> &contour, cv::Point);
    
    std::vector<std::vector< cv::Point> > shift_contours(std::vector<std::vector <cv::Point> > &contour, cv::Point);
    
    std::vector< cv::Point> merge_contours(std::vector <cv::Point>& contour1, std::vector <cv::Point>& contour2, int type);
    
    std::vector< cv::Point> intersect_contours(std::vector <cv::Point> &contour1, std::vector <cv::Point>& contour2);
    
    std::vector< cv::Point> unite_contours(std::vector <cv::Point> &contour1, std::vector <cv::Point>& contour2);
    
    cv::Mat contour_to_mask(const std::vector<std::vector<cv::Point> > &contour, const cv::Size source);
    
}


#endif // _CONTOUR_UTILS_
