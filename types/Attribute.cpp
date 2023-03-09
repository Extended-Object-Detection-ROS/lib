#include "Attribute.h"
#include <algorithm>
#include <cctype>

using namespace std;
using namespace cv;

namespace eod{
    Attribute::Attribute(){
        Weight = 1;
        parent_base = NULL;
        Probability = 0.75;
        inited = false;
        returnContours = true;
        return3D = true;
        clusterization_method = NULL;
    }
    
    void Attribute::setProbability(double prob){
        Probability = prob;
    }
    
    //TODO should not return array but take it as pointer\link
    vector<ExtendedObjectInfo> Attribute::Detect(const InfoImage& image, int seq){
        //TODO: check that all detectors cares about 'inited' param
        /*
        if( !inited ){
            return vector<ExtendedObjectInfo>(0);
        }
        */
        if( seq == 0 || seq != prev_seq ){
            saved_answer = Detect2(image, seq); // TODO [optimization] This should be changed on &, like Detect2(image, seq, saved_answer)
            for( size_t i = 0 ; i < saved_answer.size() ; i++ ){
                saved_answer[i].normalize(image.size().width, image.size().height);
            }
            if( seq != 0 )
                prev_seq = seq;
        }
        if( clusterization_method )
            saved_answer = clusterization_method->cluster(saved_answer);
                
        for( size_t i = 0; i < filters.size() ; i ++ ){
            filters[i]->Filter(&saved_answer);
        }        
        return saved_answer;
    }
    
    void Attribute::Check(const InfoImage& image, vector<ExtendedObjectInfo>* rects){        
        auto it = rects->begin();
        while (it != rects->end() ){            
            if( !Check2(image, *it) ){                
                it = rects->erase(it);
            }
            else {
                it->setScoreWeight(1, Weight);
                ++it;
            }
        }                        
    }
    
    void Attribute::Extract(const InfoImage& image, vector<ExtendedObjectInfo>* rects){
        for( size_t i = 0 ; i < rects->size() ; i++ ){            
            Extract2(image, rects->at(i));
        }
    }
    
    AttributeTypes getAttributeTypeFromName(string name){
        transform(name.begin(), name.end(), name.begin(),[](unsigned char c){ return tolower(c); });
        
        // TODO convert to std::map
        if( name == "hsvcolor" )
            return HSV_COLOR_A;
        if( name == "haarcascade" )
            return HAAR_CASCADE_A;
        if( name == "size" )
            return SIZE_A;
        if( name == "histcolor" )
            return HIST_COLOR_A;
        if( name == "hough" )
            return HOUGH_A;
        if( name == "dimension" )
            return DIMEN_A;
        if( name == "basicmotion")
            return BASIC_MOTION_A;
        if( name == "aruco")
            return ARUCO_A;
        if( name == "feature")
            return FEATURE_A;
        if ( name == "pose")
            return POSE_A;
        if ( name == "tensorflow")
            return TF_A;        
        if (name == "dnn")
            return DNN_A;
        if (name == "qr")
            return QR_A;
        if (name == "qr_zbar")
            return QR_ZBAR_A;
        if (name == "logicand")
            return LOG_AND_A;
        if (name == "logicnot")
            return LOG_NOT_A;
        if (name == "logicor")
            return LOG_OR_A;
        if (name == "logicxor")
            return LOG_XOR_A;
        if( name == "blob")
            return BLOB_A;
        if( name == "depth")
            return DEPTH_A;
        if(name == "roughdist")
            return ROUGH_DIST_A;
        if(name == "dist")
            return DIST_A;
        if(name == "facedlib")
            return FACE_DLIB_A;
        if(name == "extractedinfoid")
            return EI_ID_CHECK_A;
        if(name == "extractedinfostring")
            return EI_STR_CHECK_A;
        if(name == "unittranslation")
            return UNIT_TRANS_EXTR_A;
        if(name == "squareobjectdistance")
            return SQUARE_OBJ_DIST_EXTR_A;
        if(name == "torchyolov7")
            return TORCH_YOLOV7_A;
        
        printf("Unknown attribute type %s!",name.c_str());
        return UNK_A;
    }
    
    void Attribute::set_extracted_info(ExtendedObjectInfo &eoi, std::string key, std::string value){
        eoi.extracted_info[Name+":"+key] = value;
    }
    
    void Attribute::set_extracted_info(ExtendedObjectInfo &eoi, std::string key, int value){
        set_extracted_info(eoi, key, std::to_string(value));
    }
    
    void Attribute::set_extracted_info(ExtendedObjectInfo &eoi, std::string key, double value){
        set_extracted_info(eoi, key, std::to_string(value));
    }
        
}
