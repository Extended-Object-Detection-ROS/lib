#include "DepthDetector.h"
#include "geometry_utils.h"
#include "ObjectBase.h"
#include "contour_utils.h"

using namespace std;
using namespace cv;

namespace eod{

    DepthAttribute::DepthAttribute(){        
        inited = false;
        Type = DEPTH_A;
    }
    
    DepthAttribute::DepthAttribute(int mode_, double max_dist_m_){
        inited = true;
        Type = DEPTH_A;
        mode = mode_;
        max_dist_m = max_dist_m_;
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
                    cropped = Mat(image, rect.getRect() & rect_of_depth_image); 
                }
                else if(mode == HALF_SIZE_BOX){
                    Rect half_rect(rect.x + rect.width/4, rect.y + rect.height/4, rect.width/2, rect.height/2);
                    cropped = image(half_rect & rect_of_depth_image);                 
                }                                
                if( cropped.empty()){                
                    printf("Cropped image for DepthAttribute is empty!\n");
                    return;
                }                                            
                distance = mat_median(cropped, true, Mat(), max_dist_m*1000);                  
            }
            else if( mode == CENTER_PX ){                
                distance = image.at<char16_t>(rect.getCenter());
            }
            else if( mode == MASK){
                if( rect.contour.size() == 0){
                    printf("DepthAttribute in mode MASK(3) only works with objects that have contours!\n");
                    return;
                }                    
                Rect rect_of_depth_image = Rect(0, 0, image.size().width, image.size().height);
                Mat cropped = image(rect.getRect() & rect_of_depth_image); 
                Mat mask = contour_to_mask(shift_contours(rect.contour, rect.tl()), cropped.size());
                distance = mat_median(cropped, true, mask, max_dist_m*1000);                
            }
            else{
                printf("Unknown mode in DepthAttribute!\n");
                return;          
            }
            if( distance > 0 ){
                distance *= 0.001f; //NOTE as well depth stored as 256 char values
                if( !image.K().empty() ){
                    if( rect.tvec.size() == 0 ){
                        Vec3d tvec = get_translation(rect.getCenter(), image.K(), distance);                        
                        rect.tvec.push_back(tvec);                        
                    }
                    else{
                        for(auto& tvec : rect.tvec){
                            tvec *= distance;
                        }
                    }
                }
                else{
                    printf("Camera parameters have not been specified for DepthAttribute!\n");
                    return;
                }
            }
            else{
                printf("Object is away from depthmap! Distance = %f\n", distance);
                return;
            }                                    
        }
    }
    
}
