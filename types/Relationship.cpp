#include "Relationship.h" 

using namespace std;

namespace eod{
    
    RelationTypes getRelationTypeFromName(string name){
        transform(name.begin(), name.end(), name.begin(),[](unsigned char c){ return tolower(c); });
        
        if( name == "imagerange" )
            return IM_RANGE_R;            
        if( name == "logicand" )
            return LOG_AND_R;
        if( name == "logicnot" )
            return LOG_NOT_R;
        if( name == "logicor" )
            return LOG_OR_R;
        if( name == "3drange" )
            return TD_RANGE_R;
        if( name == "spacein" )
            return SPACE_IN_R;
        if( name == "spaceout" )
            return SPACE_OUT_R;
        if( name == "spaceup" )
            return SPACE_UP_R;
        if( name == "spacedown" )
            return SPACE_DOWN_R;
        if( name == "spaceleft" )
            return SPACE_LEFT_R;
        if( name == "spaceright" )
            return SPACE_RIGHT_R;
        if( name == "sizesame" )
            return SIZE_SAME_R;
        if( name == "sameextractedinfo" )
            return SAME_EXTR_INFO_R;
        if( name == "samedistimage")
            return SAME_DIST_IMAGE_R;
        
        printf("Unknown relation type %s!",name.c_str());
        return UNK_R;
    }
    
    RelationShip::RelationShip(){
        Type = UNK_R;
        inited = false;
        linearity = BILINEAR_R;
    }        
    
    bool RelationShip::setName(const char* name){
        if( !name )
            return false;
        Name = string(name);      
        return true;
    }

    MultiLinearRelationShip::MultiLinearRelationShip(){
        Type = UNK_R;
        inited = false;
        linearity = MULTILINEAR_R;
        graph_v_color = -1;
        prev_seq = 0;
    }

    bool MultiLinearRelationShip::checkRelation(const cv::Mat& image, ExtendedObjectInfo* A, ExtendedObjectInfo* B){
        return false;
    }

    bool MultiLinearRelationShip::get_vid_from_index(int index, int& vid){
        if( Index2VID.find(index) == Index2VID.end() ){
            return false;
        }
        else{
            vid = Index2VID[index];
            return true;
        }
    }

    void MultiLinearRelationShip::set_vid_for_index(int index, int vid){
        Index2VID.insert(std::pair<int, int>(index, vid));
    }

}
