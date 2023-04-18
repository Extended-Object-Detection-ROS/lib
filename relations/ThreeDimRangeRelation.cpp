#include "ThreeDimRangeRelation.h"
#include "geometry_utils.h"
#include "math_utils.h"

namespace eod{
    
    ThreeDimRangeRelation::ThreeDimRangeRelation(){
        inited = false;
        Type = TD_RANGE_R;
    }
    
    ThreeDimRangeRelation::ThreeDimRangeRelation(int mode, double param1, double param2){
        Type = TD_RANGE_R;
        if( mode == CLOSED_RANGE){
            sub_type = mode;
            dist_high = param2;
            dist_low = param1;
            inited = true;
        }
        else if( mode == PROB_RANGE){
            sub_type = mode;
            dist = param1;
            prob = param2;
            inited = true;
        }        
    }
    
    bool ThreeDimRangeRelation::checkRelation(const cv::Mat& image, ExtendedObjectInfo* A, ExtendedObjectInfo* B){
        if(inited){            
            if( A->tvec.size() == 0 || B->tvec.size() == 0){
                printf("Object on input to ThreeDimRangeRelation has not 3d translation yet!\n");
                return false;
            }
            if( A->tvec[0][3] == 1 || B->tvec[0][3] == 1){
                printf("Object on input to ThreeDimRangeRelation has unit 3d translation!\n");
                return false;
            }
            double distance = range_v3d(A->tvec[0], B->tvec[0]);            
            if(sub_type == CLOSED_RANGE){
                if( distance > dist_high )            
                    return false;
                if( distance < dist_low )
                    return false;
                return true;
            }
            else{
                printf("ThreeDimRangeRelation has unknown sub type!\n");
                return false;
            }
        }
        else{
            printf("ThreeDimRangeRelation isn't inited!\n");
            return false;
        }
    }
    
    double ThreeDimRangeRelation::checkSoft(const cv::Mat& image, ExtendedObjectInfo* A, ExtendedObjectInfo* B){
        if(inited){            
            if( A->tvec.size() == 0 || B->tvec.size() == 0){
                printf("Object on input to ThreeDimRangeRelation has not 3d translation yet!\n");
                return 0;
            }
            if( A->tvec[0][3] == 1 || B->tvec[0][3] == 1){
                printf("Object on input to ThreeDimRangeRelation has unit 3d translation!\n");
                return 0;
            }
            double distance = range_v3d(A->tvec[0], B->tvec[0]);            
            if(sub_type == PROB_RANGE){
                
                double score = norm_distribution(dist, distance, prob);
                //printf("%f = norm_distribution(%f, %f, %f)\n",score, dist, distance, prob);
                return score;                
            }
            else{
                printf("ThreeDimRangeRelation has unknown sub type!\n");
                return 0;
            }
        }
        else{
            printf("ThreeDimRangeRelation isn't inited!\n");
            return 0;
        }        
        return 0;
    }
    
    void ThreeDimRangeRelation::extractParams(const cv::Mat& image, ExtendedObjectInfo* A, ExtendedObjectInfo* B){
        dist = range_v3d(A->tvec[0], B->tvec[0]);
    }
    
    RelationShip* ThreeDimRangeRelation::copy(){
        ThreeDimRangeRelation* copie = new ThreeDimRangeRelation(*this);
        return (RelationShip*)copie;
    }
    
    std::string ThreeDimRangeRelation::params_as_str(){
        return std::to_string(dist);
    }
    
}
