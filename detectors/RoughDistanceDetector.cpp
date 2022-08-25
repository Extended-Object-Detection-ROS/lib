#include "RoughDistanceDetector.h"
#include "ObjectBase.h"

using namespace std;
using namespace cv;

namespace eod{
    
    RoughDistAttribute::RoughDistAttribute(){
        Type = ROUGH_DIST_A;        
        realHeight = 0;
        realWidth = 0;
        inited = false;
    }
    
    RoughDistAttribute::RoughDistAttribute(double realW, double realH){
        Type = ROUGH_DIST_A;                
        realHeight = realH;
        realWidth = realW;
        inited = true;
    }
            
    
    vector<ExtendedObjectInfo> RoughDistAttribute::Detect2(const InfoImage& image, int seq){
        return vector<ExtendedObjectInfo>();
    }
                
    bool RoughDistAttribute::Check2(const InfoImage& image, ExtendedObjectInfo& rect){
        return false;
    }
        
    void RoughDistAttribute::Extract2(const InfoImage& image, ExtendedObjectInfo& rect){        
        
        if( !image.K().empty() && !image.D().empty() ){
                 
            vector<Point2f> image_corners = rect.getCorners();            
            vector<Point3f> original_corners;
            
            if( realHeight != 0 ){
                double width = realHeight * rect.width / rect.height;
                original_corners = getOriginalCorners(width, realHeight);                
            }
            else if( realWidth != 0){
                double height = realWidth * rect.height / rect.width;
                original_corners = getOriginalCorners(realWidth, height);                
            }   
            else{
                printf("Error: realHeight and realWidth in RoughtDistance attribute are not inited!\n");
                return;
            }            
            Vec3d rvec, tvec;                          
            if( solvePnP(original_corners, image_corners, image.K(), image.D(), rvec, tvec)){
                rect.tvec.push_back(tvec);
                rect.rvec.push_back(rvec);
            }                
            else{
                printf("Error! Unable to solve PnP in RoughDistAttribute.\n");
            }
        }                
    }
    
    
    vector<Point3f> RoughDistAttribute::getOriginalCorners(double width, double height){
        vector<Point3f> corners;
        double w2 = width/2;
        double h2 = height/2;
        corners.push_back(Point3f(-w2, -h2, 0));
        corners.push_back(Point3f(w2, -h2, 0));
        corners.push_back(Point3f(w2, h2, 0));
        corners.push_back(Point3f(-w2, h2, 0));
        return corners;
    }
}

