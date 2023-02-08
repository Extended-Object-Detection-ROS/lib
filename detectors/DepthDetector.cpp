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
    
    DepthAttribute::DepthAttribute(int mode_){        
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
                distance = mat_median(cropped, true);  
            }
            else if( mode == CENTER_PX ){
                if(rect.tvec.size() == 0){
                    printf("DepthAttribute in mode CENTER_PX can't obtain distance without translation vector\n");
                    return;
                }
                if( image.K().empty() ){
                    printf("Camera parameters have not been specified for DepthAttribute!\n");
                    return;
                }
                Point center_registered = reverse_translation(rect.tvec[0], image.K());     
                                
                distance = image.at<float>(center_registered);                                
                
                /*
                printf("distance at (%i, %i): %f\n", center_registered.x, center_registered.y, distance);                
                Mat image2draw;
                cvtColor(image, image2draw, CV_GRAY2RGB);
                image2draw *= 255;
                circle(image2draw, center_registered, 10, Scalar(0, 255, 0), 3);
                imshow("depth", image2draw);
                waitKey(1);
                */
            }
            else if( mode == MASK){
                Rect rect_of_depth_image = Rect(0, 0, image.size().width, image.size().height);
                Mat cropped = image(rect.getRect() & rect_of_depth_image); 
                Mat mask = contour_to_mask(shift_contours(rect.contour, rect.tl()), cropped.size());
                distance = mat_median(cropped, true, mask);
                
            }
            else{
                printf("Unknown mode in DepthAttribute!\n");
                return;          
            }
            if( distance > 0 ){
                
                if( !image.K().empty() ){
                    if( rect.tvec.size() == 0 ){
                        Vec3d tvec = get_translation(rect.getCenter(), image.K(), distance);
                        Vec3d rvec;
                        rect.tvec.push_back(tvec);
                        //rect.rvec.push_back(rvec);
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
                printf("Object is away from depthmap!\n");
                return;
            }                                    
        }
    }
    
}
