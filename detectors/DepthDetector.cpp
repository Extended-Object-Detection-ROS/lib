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
    
    vector<ExtendedObjectInfo> DepthAttribute::Detect2(const InfoImage& image, int seq){      
        vector<ExtendedObjectInfo> objects;    
        return objects;
    }
    
    bool DepthAttribute::Check2(const InfoImage& image, ExtendedObjectInfo& rect){
        return false;
    }
    
    void DepthAttribute::Extract2(const InfoImage& image, ExtendedObjectInfo& rect){
        if( inited ){                         
            if(image.empty()){  
                printf("Depth image is not provided for DepthAttribute!\n");
                return;          
            }
            double distance = 0;
            if( mode == ALL_BOX || mode == HALF_SIZE_BOX ){
                Rect rect_of_depth_image = Rect(0, 0, image.size().width, image.size().height);
                Mat cropped;
                if( mode == ALL_BOX){                
                    cropped = image(rect.getRect() & rect_of_depth_image); 
                }
                else if(mode == HALF_SIZE_BOX){
                    Rect half_rect(rect.x + rect.width/4, rect.y + rect.height/4, rect.width/2, rect.height/2);
                    cropped = image(half_rect & rect_of_depth_image);                 
                }                                
                if( cropped.empty()){                
                    printf("Cropped image for DepthAttribute is empty!\n");
                    return;
                }                            
                distance = mat_median(cropped, true) * depth_scale;  
            }
            else if( mode == CENTER_PX ){
                if(rect.tvec.size() == 0){
                    printf("DepthAttribute in mode CENTER_PX can't obtain distance without translation vector");
                    return;
                }
            }
            else{
                printf("Unknown mode in DepthAttribute!\n");
                return;          
            }
            if( distance > 0 ){
                
                if( !image.K().empty() ){
                    Vec3d tvec = get_translation(rect.getCenter(), image.K(), distance);
                    Vec3d rvec;
                    rect.tvec.push_back(tvec);
                    rect.rvec.push_back(rvec);
                }
                else{
                    printf("Camera parameters have not been specified for DepthAttribute!\n");
                    return;
                }
            }
            else{
                printf("Object is away from depthmap!\n");
                return;
            }                                    
        }
    }
    
}
