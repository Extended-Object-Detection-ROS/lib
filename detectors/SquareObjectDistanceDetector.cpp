#include "SquareObjectDistanceDetector.h"
#include <opencv2/aruco.hpp>
#include "geometry_utils.h"

using namespace std;
using namespace cv;

namespace eod{  
    
    SquareObjectDistanceAttribute::SquareObjectDistanceAttribute(double length){
        Type = SQUARE_OBJ_DIST_EXTR_A;
        length_ = length;
    }
    
    vector<ExtendedObjectInfo> SquareObjectDistanceAttribute::Detect2(const InfoImage& image, int seq){
        return vector<ExtendedObjectInfo>(0);
    }
    
    bool SquareObjectDistanceAttribute::Check2(const InfoImage& image,ExtendedObjectInfo& rect){
                                
    }
    
    void SquareObjectDistanceAttribute::Extract2(const InfoImage& image, ExtendedObjectInfo& rect){
        if( rect.contour.size() > 0 ){
            if( rect.contour[0].size() == 4){
                if( length_ != 0 && !image.K().empty() && !image.D().empty() ){
                    //printf("Processing...\n");
                    
                    vector<cv::Vec3d> rvecs, tvecs;                         
                    
                    vector<vector<Point2f> >corners;
                    corners.push_back(int2floatPointVector(rect.contour[0]));
                    
                    cv::aruco::estimatePoseSingleMarkers(corners, length_, image.K(), image.D(), rvecs, tvecs);            
                    
                    rect.tvec.push_back(tvecs[0]);
                    rect.rvec.push_back(rvecs[0]);
                }
                else{
                    printf("No length or cam params\n");
                }
            }
            else{
                printf("Contour doesnot contain 4 points\n");
            }
        }
        else{
            printf("No contour for detecting distance\n");
        }
            
    }
    
}
