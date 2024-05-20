#ifndef _EXTENDED_OBJECT_INFO_
#define _EXTENDED_OBJECT_INFO_

#include <opencv2/opencv.hpp>
#include <vector>
#include <utility>
#include <map>

namespace eod{
    
    enum MergingPolicy {
        UNION_MP,
        INTERSECTION_MP,
    };
    
    class KeyPoint : public cv::Point {
    public:
        KeyPoint(int x, int y, float score, std::string label = "") : cv::Point(x, y){
            this->score = score;
            this->label = label;
        }
        KeyPoint(cv::Point point, float score, std::string label = "") : cv::Point(point){
            this->score = score;
            this->label = label;
        }
        
        float score;
        std::string label;
    private:        
        
    };
    
    class ExtendedObjectInfo : public cv::Rect {
    public:        
        // constructors stuff
        ExtendedObjectInfo();        
        ExtendedObjectInfo(int x, int y, int w, int h);
        ExtendedObjectInfo(cv::Rect rect);        
        ExtendedObjectInfo(cv::Rect2d rect);        
        ~ExtendedObjectInfo(){/*TODO add all pointer release*/}

        // basic rects
        const cv::Rect getRect();
        const cv::Rect2d getRect2d();
        const cv::Point getCenter();
        
        const std::vector<cv::Point2f> getCorners();
        
        // help functions
        const void print(std::string prefix = ""); 
        const std::string getInfo();        
        void draw(const cv::Mat& image, cv::Scalar col = cv::Scalar(0, 255, 0) );
        
        void normalize(int im_width, int im_height);                
        
        // operators
        friend const ExtendedObjectInfo operator& ( ExtendedObjectInfo& a,  ExtendedObjectInfo& b);
        friend const ExtendedObjectInfo operator| ( ExtendedObjectInfo& a,  ExtendedObjectInfo& b);        
        
        // merge contours, tvecs, rvecs and so on
        void mergeAllData(MergingPolicy mp = INTERSECTION_MP);
        //
        // Additional Info                        
        //
        
        // Variable behind is common for entire object
        int track_id;
        int track_status;     
        
        // score\probability collecting
        double total_score;
        void setScoreWeight(double score, double weight);
        void calcTotalScore();        
                          
        //
        // Variables behind is those, which could be individual for each attribute
        //        
        //std::vector<int> sub_id;        
        std::map<std::string, std::string> extracted_info;
        
        // for every one attribute
        std::vector<std::pair<double, double> > scores_with_weights;         
        
        // only for those attributes, which can obtain them
        std::vector<std::vector<cv::Point> > contour;
        
        // only for those attributes, which can solve them
        //translation vectors
        std::vector<cv::Vec3d> tvec;        
        // rotation vectors
        std::vector<cv::Vec3d> rvec;      
        
        // keypoints stuff
        std::vector<KeyPoint> keypoints;
        std::vector<std::pair<int, int>> keypoint_connection; 
        
        void updateRectFromKeypoints(float scale = 0);
        int getKeypointByName(std::string name);
        
        
        void initVars();       
    private:            
        void inheritData(ExtendedObjectInfo* a, ExtendedObjectInfo* b);        
                 
    };
    
    double getRange(ExtendedObjectInfo a, ExtendedObjectInfo b);    
}

#endif // _EXTENDED_OBJECT_INFO_
