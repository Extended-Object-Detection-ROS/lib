#include "ExtractedInfoStringChecker.h"
#include <algorithm>

using namespace std;
using namespace cv;

namespace eod{

	ExtractedInfoStringChecker::ExtractedInfoStringChecker(){
        Type = EI_STR_CHECK_A;
	}
	
	ExtractedInfoStringChecker::ExtractedInfoStringChecker(string field_, vector<string> allowed_, bool partially_){
        Type = EI_STR_CHECK_A;
        field = field_;
        allowed = allowed_;        
        partially = partially_;
	}
	
	vector<ExtendedObjectInfo> ExtractedInfoStringChecker::Detect2(const Mat& image, int seq){
        vector<ExtendedObjectInfo> rects(0);
        return rects;
    }
    
    void ExtractedInfoStringChecker::Extract2(const cv::Mat& image, ExtendedObjectInfo& rect){
        
    }
    
    bool ExtractedInfoStringChecker::Check2(const Mat& image, ExtendedObjectInfo& rect){              
        if( rect.extracted_info.count(field) > 0){
            string value = rect.extracted_info[field];
            
            if( allowed.size() == 0 ){
                return false;
            }
            else{
                if( ! partially ){
                    if( find(allowed.begin(), allowed.end(), value) != allowed.end() ){
                        return true;
                    }
                }
                else{
                    for(auto const& val: allowed){
                        if( value.find(val) != string::npos)
                            return true;
                    }
                }
            }
        }        
        return false;
    }    
	
}
