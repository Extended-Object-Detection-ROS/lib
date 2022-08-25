#include "HaarCascadeDetector.h"

using namespace std;
using namespace cv;

namespace eod{

    HaarCascadeAttribute::HaarCascadeAttribute(){
        Type = HAAR_CASCADE_A;
    }
    
    HaarCascadeAttribute::HaarCascadeAttribute(string name, int min_size_){
        Type = HAAR_CASCADE_A;
        cascade_name = name;
        if (!shapeClassifier.load(cascade_name)){
            printf("Error: Unable to load cacade! %s\n",cascade_name.c_str() );
            inited = false;            
        }
        inited = true;
        min_size = min_size_;
    }

    HaarCascadeAttribute::~HaarCascadeAttribute(){
        //cvReleaseHaarClassifierCascade(shapeClassifier);
    }    
    
    bool HaarCascadeAttribute::Check2(const InfoImage& image,ExtendedObjectInfo &rect){
      return false;
    }
    
    void HaarCascadeAttribute::Extract2(const InfoImage& image, ExtendedObjectInfo& rect){
    }

    vector<ExtendedObjectInfo> HaarCascadeAttribute::Detect2(const InfoImage& image, int seq){
        
        vector<ExtendedObjectInfo> res;
        vector<Rect> shapes;            
        if( inited ){
            Mat frame_gray;
#if (CV_MAJOR_VERSION > 3)            
            cvtColor(image, frame_gray, COLOR_BGR2GRAY);
#else
            cvtColor(image, frame_gray, CV_BGR2GRAY);
#endif
            equalizeHist(frame_gray, frame_gray);

            shapeClassifier.detectMultiScale(frame_gray, shapes, 1.1, 1, 0 , Size(min_size, min_size), image.size() );
                        
            for( size_t i = 0 ; i < shapes.size(); i++){                  
                ExtendedObjectInfo tmp = ExtendedObjectInfo(shapes[i]);
                tmp.setScoreWeight(1, Weight);
                res.push_back(tmp);                    
            }
        }                                          
        return res;      
    }
}
