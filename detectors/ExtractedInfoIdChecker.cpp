#include "ExtractedInfoIdChecker.h"
#include <algorithm>

using namespace std;
using namespace cv;

namespace eod{

	ExtractedInfoIdChecker::ExtractedInfoIdChecker(){
        Type = EI_ID_CHECK_A;
	}
	
	ExtractedInfoIdChecker::ExtractedInfoIdChecker(string field_, vector<int> allowed_, vector<int> forbidden_){
        Type = EI_ID_CHECK_A;
        field = field_;
        allowed = allowed_;
        forbidden = forbidden_;
	}
	
	vector<ExtendedObjectInfo> ExtractedInfoIdChecker::Detect2(const Mat& image, int seq){
        vector<ExtendedObjectInfo> rects(0);
        return rects;
    }
    
    void ExtractedInfoIdChecker::Extract2(const cv::Mat& image, ExtendedObjectInfo& rect){
        
    }
    
    bool ExtractedInfoIdChecker::Check2(const Mat& image, ExtendedObjectInfo& rect){              
        if( rect.extracted_info.count(field) > 0){
            int rect_id = stoi(rect.extracted_info[field]);
            
            if( forbidden.size() != 0){
                if( find(forbidden.begin(), forbidden.end(), rect_id) != forbidden.end() ){
                    return false;
                }
            }
            
            if( allowed.size() == 0 ){
                return true;
            }
            else{
                if( find(allowed.begin(), allowed.end(), rect_id) != allowed.end() ){
                    return true;
                }
            }
        }        
        return false;
    }    
	
}
