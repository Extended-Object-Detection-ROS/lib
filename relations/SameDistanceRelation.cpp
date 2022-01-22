#include "SameDistanceRelation.h"
#include "geometry_utils.h"

namespace eod {

    SameDistImageRelation::SameDistImageRelation(){
        Type = SAME_DIST_IMAGE_R;
    }

    SameDistImageRelation::SameDistImageRelation(double dist_error) : MultiLinearRelationShip(){
        Type = SAME_DIST_IMAGE_R;
        error = dist_error;
    }

    std::vector<int> SameDistImageRelation::checkMultilinearRelation(const cv::Mat& image, int seq, const ExtendedObjectInfo& A, const ExtendedObjectInfo& B){
        if( seq == 0 || seq != prev_seq){
            distances.clear();
            prev_seq = seq;
        }

        std::vector<int> indexes;
        double dist = rect_distance(A, B);
        if (dist == 0)
            return indexes;

        for( int i = 0 ; i < distances.size() ; i++){
            if(fabs(distances[i]-dist)/std::max(distances[i], dist) < error ){
                indexes.push_back(i);
            }
        }
        // if none found append this
        if( indexes.size() == 0){
            distances.push_back(dist);
            indexes.push_back(distances.size() -1);
        }
        return indexes;
    }


}
