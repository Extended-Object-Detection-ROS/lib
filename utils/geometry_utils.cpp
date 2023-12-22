#include "geometry_utils.h"
#include "drawing_utils.h"

using namespace cv;
using namespace std;

namespace eod{
    
    
    Rect2d rect2rect2d(Rect src){
        return Rect2d(src.x, src.y, src.width, src.height);        
    }
    
    Rect rect2d2rect(Rect2d src){
        return Rect(src.x, src.y, src.width, src.height);        
    }
    
    double intersectionOverUnion(Rect* box1, Rect* box2){                
        int intersectArea = (*box1 & *box2).area();
        int unionArea = abs(box1->area()) + abs(box2->area()) - intersectArea;

        return (double(intersectArea)) / unionArea;        
    }
    
    // thanks to https://gist.github.com/shubh-agrawal/76754b9bfb0f4143819dbd146d15d4c8
    void getQuaternion(Mat R, double Q[])
    {
        double trace = R.at<double>(0,0) + R.at<double>(1,1) + R.at<double>(2,2);
    
        if (trace > 0.0) 
        {
            double s = sqrt(trace + 1.0);
            Q[3] = (s * 0.5);
            s = 0.5 / s;
            Q[0] = ((R.at<double>(2,1) - R.at<double>(1,2)) * s);
            Q[1] = ((R.at<double>(0,2) - R.at<double>(2,0)) * s);
            Q[2] = ((R.at<double>(1,0) - R.at<double>(0,1)) * s);
        } 
        
        else 
        {
            int i = R.at<double>(0,0) < R.at<double>(1,1) ? (R.at<double>(1,1) < R.at<double>(2,2) ? 2 : 1) : (R.at<double>(0,0) < R.at<double>(2,2) ? 2 : 0); 
            int j = (i + 1) % 3;  
            int k = (i + 2) % 3;

            double s = sqrt(R.at<double>(i, i) - R.at<double>(j,j) - R.at<double>(k,k) + 1.0);
            Q[i] = s * 0.5;
            s = 0.5 / s;

            Q[3] = (R.at<double>(k,j) - R.at<double>(j,k)) * s;
            Q[j] = (R.at<double>(j,i) + R.at<double>(i,j)) * s;
            Q[k] = (R.at<double>(k,i) + R.at<double>(i,k)) * s;
        }
    }
    
    vector<Point3f> addZeroZ(vector<Point2f> src){
        vector<Point3f> dst;
        for( size_t i = 0 ; i < src.size() ; i++ ){
            Point3f tmp;
            tmp.x = src[i].x;
            tmp.y = src[i].y;
            tmp.z = 0;
            dst.push_back(tmp);
        }
        return dst;
    }
    
    Vec3d scaleVec3d(Vec3d src, double scale){
        Vec3d dst;
        dst[0] = src[0] * scale;
        dst[1] = src[1] * scale;
        dst[2] = src[2] * scale;
        return dst;
    }
    
    cv::Point float2intPoint(const cv::Point2f& src){
        Point dst;
        dst.x = (int)src.x;
        dst.y = (int)src.y;
        return dst;
    }
    
    cv::Point2f int2floatPoint(const cv::Point& src){
        Point2f dst;
        dst.x = src.x;
        dst.y = src.y;
        return dst;
    }
    
    std::vector<Point> float2intPointVector(std::vector<cv::Point2f> src){
        vector<Point> dst;
        for( size_t i = 0 ; i < src.size() ; i++ ){
            dst.push_back(float2intPoint(src.at(i)));
        }
        return dst;            
    }
    
    std::vector<Point2f> int2floatPointVector(std::vector<cv::Point> src){
        vector<Point2f> dst;
        for( size_t i = 0 ; i < src.size() ; i++ ){
                dst.push_back(int2floatPoint(src.at(i)));
        }
        return dst;            
    }
    
    float range_2f(Point2f a, Point2f b){
        return sqrt( pow((a.x -b.x),2) + pow((a.y - b.y),2) );
    }

    double range(Point a, Point b){
        return sqrt( pow(a.x - b.x,2) + pow(a.y - b.y,2) );
    }    
    
    bool vec3d_empty(cv::Vec3d src){
        if( src[0] == 0 && src[1] == 0 && src[2] == 0 )
            return true;
        return false;
    }
    
    bool rect_inside(cv::Rect a, cv::Rect b){
        if( a == b )
            return false;
        if( a.x < b.x )
            return false;
        if( a.y < b.y )
            return false;
        if( a.x + a.width > b.x + b.width )
            return false;
        if( a.y + a.height > b.y + b.height )
            return false;
        return true;
    }
    
    Mat createClosenessMap(vector<ExtendedObjectInfo>* rect1, vector<ExtendedObjectInfo>* rect2, double iou_threshold_d){
        Mat_<double> closenessMapD = Mat_<double>(rect1->size(), rect2->size());        
        for(int i = 0; i < rect1->size(); i++){            
            for(int j = 0; j < rect2->size(); j++){
                double closeness = intersectionOverUnion(&(rect1->at(i)) ,&(rect2->at(j)) );
                closenessMapD[i][j] = closeness >= iou_threshold_d ? closeness : 0;
            }
        }
        return closenessMapD;
    }
    
    double mat_median(const cv::Mat& channel, bool mask_zeros, cv::Mat custom_mask, int max_dist_cm){
        
        Mat mask = Mat::zeros(channel.size(), CV_8UC1);        
        if( mask_zeros ){
            cv::inRange( channel, 0, 0, mask);
            cv::bitwise_not(mask, mask);
        }
        if( !custom_mask.empty())
            cv::bitwise_and(mask, custom_mask, mask);                
                
        int m = countNonZero(mask)/2;
        int bin = 0;
        double med = -1.0;

        int histSize = std::min(max_dist_cm, 65536);//16^2
        float range[] = {0, histSize};
        const float* histRange = {range};
        bool uniform = true;
        bool accumulate = false;
        cv::Mat hist;                        
        cv::calcHist( &channel, 1, 0, mask, hist, 1, &histSize, &histRange, uniform, accumulate );
        
        for( int i = 0; i < histSize && med < 0.0; ++i ){
            bin += cvRound( hist.at<float>(i) );
            if( bin > m && med < 0.0 ){
                med = i;
            }
        }
        return med;
    }
    
    Vec3d get_translation(const Point& point, const Mat& camMat, double dist){
        Vec3d tvec;
        tvec[0] = dist * (point.x - camMat.at<double>(0,2)) / camMat.at<double>(0,0);
        tvec[1] = dist * (point.y - camMat.at<double>(1,2)) / camMat.at<double>(1,1);
        tvec[2] = dist;
        return tvec;
    }
    
    Vec3d get_translation_oak_fixed(const Point& point, const Mat& camMat, double dist){
        Vec3d tvec;
        tvec[0] = (point.x - camMat.at<double>(0,2)) / camMat.at<double>(0,0);
        tvec[1] = (point.y - camMat.at<double>(1,2)) / camMat.at<double>(1,1);
        tvec[2] = 1;
        
        double r = sqrt( pow(tvec[0], 2) + pow(tvec[1], 2) + pow(tvec[2],2));
        
        double r_fixed = dist / r;
        
        return get_translation(point, camMat, r_fixed);
    }
    
    Point reverse_translation(const Vec3d& translation, const Mat& camMat){
        int x = int(((translation[0] * camMat.at<double>(0,0))/translation[2]) + camMat.at<double>(0,2));
        int y = int(((translation[1] * camMat.at<double>(1,1))/translation[2]) + camMat.at<double>(1,2));
        return Point(x, y);        
    }
    
    void reverse_translation(cv::Point& pt, const Vec3d& translation, const Mat& camMat){
        pt.x = int(((translation[0] * camMat.at<double>(0,0))/translation[2]) + camMat.at<double>(0,2));
        pt.y = int(((translation[1] * camMat.at<double>(1,1))/translation[2]) + camMat.at<double>(1,2));        
    }
    
    
    double range_v3d(Vec3d a, Vec3d b){
        double dx = a[0] - b[0];
        double dy = a[1] - b[1];
        double dz = a[2] - b[2];
        return sqrt( dx*dx + dy*dy + dz*dz);
    }
    
    // shortest distance between borders
    double rect_distance(Rect r1, Rect r2){
        bool left = r2.br().x < r1.tl().x;
        bool right = r1.br().x < r2.tl().x;
        bool bottom = r2.br().y < r1.tl().y;
        bool top = r1.br().y < r2.tl().y; 
        
        if( top && left )
            return range(Point(r1.tl().x, r1.br().y), Point(r2.br().x, r2.tl().y));
        if( left && bottom )
            return range(r1.tl(), r2.br());
        if( bottom && right )
            return range(Point(r1.br().x, r1.tl().y), Point(r2.tl().x, r2.br().y));
        if( right && top )
            return range(r1.br(), r2.tl());
        if( left )
            return r1.tl().x - r2.br().x;
        if( right )
            return r2.tl().x - r1.br().x;
        if( bottom )
            return r1.tl().y - r2.br().y;
        if( top )
            return r2.tl().y - r1.br().y;
        return 0;
    }
    
    cv::Point transform_between_channels(const cv::Point& point, const cv::Mat& src, const cv::Mat& dst){
        auto tvec = get_translation(point, src);
        return reverse_translation(tvec, dst);         
    }
    
    void transform_between_channels_(cv::Point& point, const cv::Mat& src, const cv::Mat& dst){
        auto tvec = get_translation(point, src);
        reverse_translation(point, tvec, dst);         
    }
    
    cv::Point scale_point(const cv::Point& point, const cv::Size& old_size, const cv::Size& new_size){
        cv::Point new_point;
        new_point.x = int((float)point.x / old_size.width * new_size.width);
        new_point.y = int((float)point.y / old_size.height * new_size.height);
        return new_point;
    }
    
    cv::Vec<int,2> substruct_points_to_vector(cv::Point a, cv::Point b){
        
        cv::Vec<int,2> result;
        result[0] = a.x - b.x;
        result[1] = a.y - b.y;
        return result;
    }
    
        
    
}
