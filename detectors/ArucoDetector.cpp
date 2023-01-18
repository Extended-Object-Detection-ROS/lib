#include "ArucoDetector.h"
#include "ObjectBase.h"
#include "geometry_utils.h"
#include <typeinfo>

using namespace std;
using namespace cv;

namespace eod{    
        
    ArucoAttribute::ArucoAttribute(){     
        Type = ARUCO_A;        
        inited = false;
    }
    
    ArucoAttribute::ArucoAttribute(int dictionary_, double markerLen_){
        if( dictionary_ < 0 ){
            dict = cv::aruco::DICT_4X4_50;
            printf("Aruco dictionary didin't suport negative values, dictionary is set to  default 4x4_50\n");
        }
        else if( dictionary_ >  20){
            dict = cv::aruco::DICT_4X4_50;
            printf("Aruco dictionary cant be more than 20, dictionary is set to  default 4x4_50\n");
        }
        else
            dict = cv::aruco::PREDEFINED_DICTIONARY_NAME(dictionary_);
        
        markerLen = markerLen_;
                
        Type = ARUCO_A;                        
        dictionary = cv::aruco::getPredefinedDictionary(dict);        
        inited = true;        
    }    
    
    vector<ExtendedObjectInfo> ArucoAttribute::Detect2(const InfoImage& image, int seq){
        if( !inited )
            return vector<ExtendedObjectInfo>(0);                
                        
        vector<ExtendedObjectInfo> rects;        
        if( seq == 0 || seq != prev_seq ){            
            cv::aruco::detectMarkers(image, dictionary, markerCorners, markerIds);
        }        
        for( size_t i = 0 ; i < markerIds.size(); i++ ){            
            ExtendedObjectInfo tmp = boundingRect( markerCorners[i] );                                                                  
            set_extracted_info(tmp, "marker_id", markerIds[i]);            
            if( returnContours )
                tmp.contour.push_back(float2intPointVector(markerCorners[i]));                        
            
            // if image has params
            if( markerLen > 0 && !image.K().empty() && !image.D().empty() ){                                
                vector<cv::Vec3d> rvecs, tvecs;                    
                cv::aruco::estimatePoseSingleMarkers(vector<vector<Point2f> >(markerCorners.begin()+i,markerCorners.begin()+i+1), markerLen, image.K(), image.D(), rvecs, tvecs);            
                tmp.tvec.push_back(tvecs[0]);
                tmp.rvec.push_back(rvecs[0]);
            }
            
            tmp.setScoreWeight(1, Weight);//TODO use reprojection to calc 
            rects.push_back(tmp);                
            
        }
        if( seq != 0)
            prev_seq = seq;
        return rects;       
    }          
        
    bool ArucoAttribute::Check2(const InfoImage& image,ExtendedObjectInfo& rect){
        return false;        
    }
    
    void ArucoAttribute::Extract2(const InfoImage& image, ExtendedObjectInfo& rect){
    }
    
    
}
