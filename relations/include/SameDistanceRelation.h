#ifndef SAME_DIST_RELATION_N
#define SAME_DIST_RELATION_N

#include "Relationship.h"

namespace eod{

    // IMAGE

    class SameDistImageRelation : public MultiLinearRelationShip{
    public:
        SameDistImageRelation();
        SameDistImageRelation(double dist_error);

        std::vector<int> checkMultilinearRelation(const cv::Mat& image, int seq, const ExtendedObjectInfo& A, const ExtendedObjectInfo& B);

        SameDistImageRelation * clone () const
        {
            return new SameDistImageRelation(*this);
        }

    private:
        double error;

        std::vector<double> distances;
    };

    // 3D

}
#endif //SAME_DIST_RELATION_N
