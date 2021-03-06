#include "DepthDetector.h"
#include "geometry_utils.h"
#include "ObjectBase.h"

using namespace std;
using namespace cv;

namespace eod{

    DepthAttribute::DepthAttribute(){
        depth_scale = 0.0;
        inited = false;
        Type = DEPTH_A;
    }
    
    DepthAttribute::DepthAttribute(double depth_scale_, int mode_){
        depth_scale = depth_scale_;
        inited = true;
        Type = DEPTH_A;
        mode = mode_;
    }
    
    vector<ExtendedObjectInfo> DepthAttribute::Detect2(const Mat& image, int seq){      
        vector<ExtendedObjectInfo> objects;    
        return objects;
    }
    
    bool DepthAttribute::Check2(const cv::Mat& image, ExtendedObjectInfo& rect){
        return false;
    }
    
    void DepthAttribute::Extract2(const cv::Mat& image, ExtendedObjectInfo& rect){
        if( inited ){                         
            if(image.empty()){  
                printf("Depth image is not provided for DepthAttribute!\n");
                return;          
            }
            Rect rect_of_depth_image = Rect(0, 0, image.size().width, image.size().height);
            Mat cropped;
            if( mode == ALL_BOX){                
                cropped = image(rect.getRect() & rect_of_depth_image); 
            }
            else if(mode == HALF_SIZE_BOX){
                Rect half_rect(rect.x + rect.width/4, rect.y + rect.height/4, rect.width/2, rect.height/2);
                cropped = image(half_rect & rect_of_depth_image);                 
            }
            else{
                printf("Unknown mode in DepthAttribute!\n");
                return;          
            }
                        
            double distance = mat_median(cropped, true) * depth_scale;  
            if( distance > 0 ){
                Mat camMat = parent_base->getCameraMatrix(); 
                if( !camMat.empty() ){
                    Vec3d tvec = get_translation(rect.getCenter(), camMat, distance);
                    Vec3d rvec;
                    rect.tvec.push_back(tvec);
                    rect.rvec.push_back(rvec);
                }
                else{
                    printf("Camera parameters have not been specified for DepthAttribute!\n");
                }
            }
            else{
                printf("Object is away from depthmap!\n");
            }
            
            
            
        }
    }
    
}
