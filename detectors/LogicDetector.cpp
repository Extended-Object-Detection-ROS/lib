#include "LogicDetector.h"
#include "geometry_utils.h"

using namespace std;
using namespace cv;

namespace eod{
    
    // ------------------------
    // AND
    // ------------------------
    AndAttribute::AndAttribute(){
        attributeA = NULL;
        attributeB = NULL;
        iou_threshold = 0.75;
        inited = false;
        second_check = false;
        Type = LOG_AND_A;
    }
        
    AndAttribute::AndAttribute(Attribute* a, Attribute* b, double iou_thesh, bool second_check_){
        attributeA = a;
        attributeB = b;
        iou_threshold = iou_thesh;
        inited = true;
        second_check = second_check_;
        Type = LOG_AND_A;
    }
    
    vector<ExtendedObjectInfo> AndAttribute::Detect2(const InfoImage& image, int seq){                
        vector<ExtendedObjectInfo> objects;    
        if(!inited) return objects;
        vector<ExtendedObjectInfo> rectsA = attributeA->Detect2(image, seq);
        
        if( !second_check ){
            vector<ExtendedObjectInfo> rectsB = attributeB->Detect2(image, seq);                
                
            if( rectsA.size() == 0 || rectsB.size() == 0)
                return objects;
                    
            Mat_<double> closenessMapD = createClosenessMap(&rectsA, &rectsB, iou_threshold);
            
            Mat mask(closenessMapD.size(), CV_8UC1, Scalar(255,255,255));            
            
            while( true ){
                double min, max = 0;
                Point min_loc, max_loc;
                minMaxLoc(closenessMapD, &min, &max, &min_loc, &max_loc, mask);
                if( max == 0 ) break;                                           
                ExtendedObjectInfo newone = rectsA[max_loc.y] & rectsB[max_loc.x];                        
                // TODO: here may be diffrent policy of inner information merging, rather different than for simple objects
                objects.push_back(newone); 
                mask.row(max_loc.y).setTo(Scalar(0,0,0));                                    
                mask.col(max_loc.x).setTo(Scalar(0,0,0));                                
            }
            mask.release();
            closenessMapD.release();
                        
            return objects;
        }
        else{
            auto it = rectsA.begin();
            while (it != rectsA.end() ){            
                if( !attributeB->Check2(image, *it) ){                
                    it = rectsA.erase(it);
                }
                else {
                    //it->setScoreWeight(1, Weight);
                    ++it;
                }
            }    
            return rectsA;
        }
        
    }
    
    bool AndAttribute::Check2(const InfoImage& image, ExtendedObjectInfo& rect){
        return attributeA->Check2(image, rect) && attributeB->Check2(image, rect);
    }
    
    void AndAttribute::Extract2(const InfoImage& image, ExtendedObjectInfo& rect){
    }
    
    // ------------------------
    // NOT
    // ------------------------    
    NotAttribute::NotAttribute(){
        attribute = NULL;                
        inited = false;
        Type = LOG_NOT_A;
        method = NotAttributeMethod::NAM_CHECK;
    }
    
    NotAttribute::NotAttribute(Attribute* a, NotAttributeMethod method_, double iou_){
        attribute = a;        
        inited = true;
        Type = LOG_NOT_A;
        method = method_;
        iou = iou_;
    }
    
    vector<ExtendedObjectInfo> NotAttribute::Detect2(const InfoImage& image, int seq){                
        vector<ExtendedObjectInfo> objects;                                    
        vector<ExtendedObjectInfo> rects = attribute->Detect2(image, seq);
        if( rects.size() == 0 ){
            ExtendedObjectInfo tmp = ExtendedObjectInfo(0, 0, image.cols, image.rows);
            objects.push_back(tmp);
        }            
        //TODO:        
        // will think what else could be there, maybe some kind of inverted rects
        // or take maximal possible rect that not contains detected ones        
        return objects;
    }
    
    bool NotAttribute::Check2(const InfoImage& image, ExtendedObjectInfo& rect){
        if( method == NotAttributeMethod::NAM_CHECK){
            bool rv = !attribute->Check2(image, rect);
            if( rv == false){
                rect.scores_with_weights.pop_back();
            }
            else{
                rect.setScoreWeight(1, Weight);
            }
            return rv;
        }
        if( method == NotAttributeMethod::NAM_DETECT){
            vector<ExtendedObjectInfo> rects = attribute->Detect2(image, image.seq());
            for( auto& eoi : rects ){
                if( intersectionOverUnion(&rect, &eoi) >= iou ){
                    return false;
                }
            }
            return true;
        }
        printf("Error in NotAttribute, unknown value %i\n", method);
        return false;
    }    
    
    void NotAttribute::Extract2(const InfoImage& image, ExtendedObjectInfo& rect){
    }
    
    // ------------------------
    // OR
    // ------------------------
    OrAttribute::OrAttribute(){
        attributeA = NULL;
        attributeB = NULL;
        iou_threshold = 0.75;
        inited = false;
        Type = LOG_OR_A;
    }
        
    OrAttribute::OrAttribute(Attribute* a, Attribute* b, double iou_thesh){
        attributeA = a;
        attributeB = b;
        iou_threshold = iou_thesh;
        inited = true;
        Type = LOG_OR_A;
    }
    
    vector<ExtendedObjectInfo> OrAttribute::Detect2(const InfoImage& image, int seq){                
        vector<ExtendedObjectInfo> objects;    
        if(!inited) return objects;
        
        vector<ExtendedObjectInfo> rectsA = attributeA->Detect2(image, seq);
        vector<ExtendedObjectInfo> rectsB = attributeB->Detect2(image, seq);                
        
        if( rectsA.size() == 0 ){
            return rectsB;
        }
        if( rectsB.size() == 0 ){
            return rectsA;
        }
        
        Mat_<double> closenessMapD = createClosenessMap(&rectsA, &rectsB, iou_threshold);
        
        Mat mask(closenessMapD.size(), CV_8UC1, Scalar(255,255,255));            
        
        while( true ){
            double min, max = 0;
            Point min_loc, max_loc;
            minMaxLoc(closenessMapD, &min, &max, &min_loc, &max_loc, mask);
            if( max != 0 ){                
                ExtendedObjectInfo newone = rectsA[max_loc.y] & rectsB[max_loc.x];                        
                objects.push_back(newone); 
                mask.row(max_loc.y).setTo(Scalar(0,0,0));                                    
                mask.col(max_loc.x).setTo(Scalar(0,0,0));                                
            }
            else{
                objects.push_back(rectsA[max_loc.y]);
                objects.push_back(rectsB[max_loc.x]);
                mask.row(max_loc.y).setTo(Scalar(0,0,0));                                    
                mask.col(max_loc.x).setTo(Scalar(0,0,0));                                
            }
            //exit from cycle
            minMaxLoc(mask, &min, &max, &min_loc, &max_loc );
            if( max == 0 )
                break;
        }
        
        mask.release();
        closenessMapD.release();
        
        return objects;
    }
    
    bool OrAttribute::Check2(const InfoImage& image, ExtendedObjectInfo& rect){
        bool val = attributeA->Check2(image, rect) || attributeB->Check2(image, rect);
        rect.setScoreWeight(1, Weight);
        return val;
    }
    
    void OrAttribute::Extract2(const InfoImage& image, ExtendedObjectInfo& rect){
    }
    
    // ------------------------
    // XOR
    // ------------------------
}
