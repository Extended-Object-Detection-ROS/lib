#ifndef _GEOMETRY_UTILS_
#define _GEOMETRY_UTILS_

#include <opencv2/opencv.hpp>
#include <math.h>
#include <vector>
#include "ExtendedObjectInfo.h"

namespace eod{
    
    cv::Rect2d rect2rect2d(cv::Rect src);
    
    cv::Rect rect2d2rect(cv::Rect2d src);
    
    double intersectionOverUnion(cv::Rect* box1, cv::Rect* box2);
    
    void getQuaternion(cv::Mat R, double Q[]);

    std::vector<cv::Point3f> addZeroZ(std::vector<cv::Point2f> src);
    
    cv::Vec3d scaleVec3d(cv::Vec3d src, double scale);
    
    cv::Point float2intPoint(const cv::Point2f&);
    
    cv::Point2f int2floatPoint(const cv::Point&);
    
    std::vector<cv::Point> float2intPointVector(std::vector<cv::Point2f>);
    
    std::vector<cv::Point2f> int2floatPointVector(std::vector<cv::Point>);
    
    float range_2f(cv::Point2f a, cv::Point2f b);
    
    double range(cv::Point a, cv::Point b);
    
    bool vec3d_empty(cv::Vec3d);
    
    bool rect_inside(cv::Rect, cv::Rect);
    
    cv::Mat createClosenessMap(std::vector<ExtendedObjectInfo>* rect1, std::vector<ExtendedObjectInfo>* rect2, double iou_threshold_d);                
 
    double mat_median( const cv::Mat& channel, bool mask_zeros = false,  cv::Mat custom_mask = cv::Mat(), int max_dist_cm = 12000);
    
    cv::Vec3d get_translation(const cv::Point& point, const cv::Mat& camMat, double dist = 1.0);
    
    cv::Point reverse_translation(const cv::Vec3d&, const cv::Mat& camMat);
    
    double range_v3d(cv::Vec3d, cv::Vec3d);
    
    double rect_distance(cv::Rect, cv::Rect);
        
    cv::Point transform_between_channels(const cv::Point& point, const cv::Mat& src, const cv::Mat& dst);
    
    cv::Point scale_point(const cv::Point& point, const cv::Size& old_size, const cv::Size& new_size);
    
}

#endif
