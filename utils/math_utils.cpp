#include "math_utils.h"
#include <cmath>

namespace eod{

    double normal_pdf(double x, double m, double s){
        static const double inv_sqrt_2pi = 0.3989422804014327;
        double a = (x - m) / s;
        return inv_sqrt_2pi / s * std::exp(-0.5f * a * a);
    }
    
    
    double norm_distribution(double x, double m, double s){        
        double a = (x - m) / s;
        return std::exp(-0.5f * a * a);
    }
    
}
