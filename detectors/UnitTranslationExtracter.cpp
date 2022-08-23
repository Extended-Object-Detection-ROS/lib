#include "UnitTranslationExtracter.h"
#include "geometry_utils.h"

using namespace std;
using namespace cv;

namespace eod{

	UnitTranslationExtracter::UnitTranslationExtracter(){
        Type = UNIT_TRANS_EXTR_A;
	}
	
	vector<ExtendedObjectInfo> UnitTranslationExtracter::Detect2(const InfoImage& image, int seq){
        vector<ExtendedObjectInfo> rects(0);
        return rects;
    }
    
    void UnitTranslationExtracter::Extract2(const InfoImage& image, ExtendedObjectInfo& rect){
        if( rect.tvec.size() > 0){
            printf("UnitTranslationExtracter: given rect already has translation, skip");
            return;
        }
        if( image.K().empty()){
            printf("No camera params are specified for UnitTranslationExtracter");
            return;
        }        
        rect.tvec.push_back(get_translation(rect.getCenter(), image.K(), 1));                
    }
    
    bool UnitTranslationExtracter::Check2(const InfoImage& image, ExtendedObjectInfo& rect){                      
        return false;
    }    
	
}
