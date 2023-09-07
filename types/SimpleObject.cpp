#include "SimpleObject.h"
#include "geometry_utils.h"
#include "drawing_utils.h"

using namespace std;
using namespace cv;

namespace eod{

    //-----------------------------------------------------------------
    // Object constructor
    //-----------------------------------------------------------------
    
    SimpleObject::SimpleObject(){
        defaultInit();        
    }

    SimpleObject::SimpleObject(string name_){
        defaultInit();
        name = name_;
    }
    
    void SimpleObject::defaultInit(){
        //TODO remove depricated params
        identified = false;
        ident_seq = -1;
        totalWeight = 0;
        borderPc = 0.02;
        image_samples = 0;       
        identify_mode = HARD;        
        merging_policy = INTERSECTION_MP;
    }

    //-----------------------------------------------------------------
    // SimpleObject interfaces
    //-----------------------------------------------------------------
    
    void SimpleObject::AddModeAttribute(AttributeMode mode, AttributeChannel channel, Attribute* attr){
        mode_attributes.push_back(make_pair(make_pair(mode, channel), attr));
    }
    
    //-----------------------------------------------------------------
    // Handle instruments
    //-----------------------------------------------------------------

    void SimpleObject::printInfo(){
    }

    string SimpleObject::getInfoStr(){
        string infoStr;
        return infoStr;
    }
    
    //-----------------------------------------------------------------
    // Ordinary Detection Stuff
    //-----------------------------------------------------------------
    vector<ExtendedObjectInfo> SimpleObject::Identify(const InfoImage& frame, const InfoImage& depth, int seq){
        if (seq == ident_seq){
            return objects;
        }
        
        if( identify_mode == HARD ){
            IdentifyHard(frame, depth, seq);
        }
        else if( identify_mode == SOFT ){
            IdentifySoft(frame, depth, seq);
        }        
        ident_seq = seq;
        
        for( size_t i = 0; i < filters.size() ; i ++ ){
            filters[i]->Filter(&objects);
        }
        return objects;
    }
    
    vector<ExtendedObjectInfo> SimpleObject::IdentifyHard(const InfoImage& frame, const InfoImage& depth, int seq){
        // speed up fo only for one attribute objects
        if( mode_attributes.size() == 1 ){
            objects.clear(); // need I this?
            if( mode_attributes[0].first.first == DETECT ){
                if( mode_attributes[0].first.second == RGB)
                    objects = mode_attributes[0].second->Detect(frame, seq);
                else if( mode_attributes[0].first.second == DEPTH)
                    objects = mode_attributes[0].second->Detect(depth, seq);
//                 for( size_t i = 0 ; i < objects.size(); i++)
//                     //objects[i].calcTotalScore();                
//                     objects[i].mergeAllData(merging_policy);
            }
            // else TODO how about to use checker on whole image in such case
//             auto it = objects.begin();
//             while (it != objects.end() ){   
//                 //it->calcTotalScore();
//                 it->mergeAllData(merging_policy);
//                 if( it->total_score < Probability ){
//                     it = objects.erase(it);
//                 }
//                 else
//                     ++it;
//             }    
//             return objects;
        }
        else{
            // more than one
            vector<ExtendedObjectInfo> prev;
            for (size_t i = 0; i < mode_attributes.size(); i++){
                // transform EOI to another channel coords if needed
                if( i != 0 && mode_attributes[i].first.second != mode_attributes[i-1].first.second){
                    if( mode_attributes[i].first.second == RGB)
                        transform_eoi(objects, depth, frame);
                    else
                        transform_eoi(objects, frame, depth);
                }            
                // do detect, check and extract
                if( mode_attributes[i].first.first == DETECT ){
                    vector<ExtendedObjectInfo> rects;
                    if( mode_attributes[i].first.second == RGB)
                        rects = mode_attributes[i].second->Detect(frame, seq);
                    else if( mode_attributes[i].first.second == DEPTH)
                        rects = mode_attributes[i].second->Detect(depth, seq);
                    
                    if( rects.size()  == 0 ){
                        objects.clear();
                        return objects;
                    }
                    if( i == 0 ){
                        prev = rects;
                        objects = rects;
                    }
                    else{
                        prev = objects;   
                        Mat_<double> closenessMapD = createClosenessMap(&prev, &rects, iou_threshold_d);
                        Mat mask(closenessMapD.size(), CV_8UC1, Scalar(255,255,255));            
                        objects.clear();
                        while( true ){
                            double min, max = 0;
                            Point min_loc, max_loc;
                            minMaxLoc(closenessMapD, &min, &max, &min_loc, &max_loc, mask);
                            if( max == 0 ) break;                               
                                
                            ExtendedObjectInfo newone;
                            if( merging_policy == INTERSECTION_MP )
                                newone = prev[max_loc.y] & rects[max_loc.x];   
                            else if( merging_policy == UNION_MP )
                                newone = prev[max_loc.y] | rects[max_loc.x];   
                            
                            objects.push_back(newone); 
                            mask.row(max_loc.y).setTo(Scalar(0,0,0));                                    
                            mask.col(max_loc.x).setTo(Scalar(0,0,0));                                
                        }
                        mask.release();
                        closenessMapD.release();
                        if( objects.size() == 0 ){                    
                            return objects;
                        }
                        prev.clear();                                 
                    }
                }
                else if(mode_attributes[i].first.first == CHECK){              
                    if( mode_attributes[i].first.second == RGB)
                        mode_attributes[i].second->Check(frame, &objects);
                    else if( mode_attributes[i].first.second == DEPTH)
                        mode_attributes[i].second->Check(depth, &objects);
                    if( objects.size() == 0 ){
                        return objects;                           
                    }
                }           
                else if(mode_attributes[i].first.first == EXTRACT){                   
                    if( mode_attributes[i].first.second == RGB)
                        mode_attributes[i].second->Extract(frame, &objects);
                    else if( mode_attributes[i].first.second == DEPTH)
                        mode_attributes[i].second->Extract(depth, &objects);
                }
            }
        }
        
        // final process
        for( size_t i = 0 ; i < objects.size(); i++){
            objects[i].mergeAllData(merging_policy);
            auto it = objects.begin();
            while (it != objects.end() ){                
                it->mergeAllData(merging_policy);
                if( it->total_score < Probability ){
                    it = objects.erase(it);
                }
                else
                    ++it;
            }
        }
        // reconvert back to RGB
        if( objects.size() > 0 ){
            if( mode_attributes.back().first.second != RGB ){
                transform_eoi(objects, depth, frame);
            }
        }
        //printf("OBJECTS DONE\n");
        return objects;
    }
    
    vector<ExtendedObjectInfo> SimpleObject::IdentifySoft(const InfoImage& frame, const InfoImage& depth, int seq){
        ExtendedObjectInfo fake_empty_rect = ExtendedObjectInfo(0,0,frame.size().width, frame.size().height);
        vector<ExtendedObjectInfo> prev;
        
        objects.clear();
        for (size_t i = 0; i < mode_attributes.size(); i++){     
            // transform EOI to another channel coords if needed
            if( i != 0 && mode_attributes[i].first.second != mode_attributes[i-1].first.second){
                if( mode_attributes[i].first.second == RGB)
                    transform_eoi(objects, depth, frame);
                else
                    transform_eoi(objects, frame, depth);
            }
            // do detect, check and extract
            if( mode_attributes[i].first.first == DETECT ){                                
                if( i == 0 ){
                    if( mode_attributes[0].first.second == RGB)
                        objects = mode_attributes[0].second->Detect(frame, seq);
                    else if( mode_attributes[0].first.second == DEPTH)
                        objects = mode_attributes[0].second->Detect(depth, seq);
                }
                else{
                    prev = objects;                
                    vector<ExtendedObjectInfo> rects;
                    if( mode_attributes[i].first.second == RGB)
                        rects = mode_attributes[i].second->Detect(frame, seq);
                    else if( mode_attributes[i].first.second == DEPTH)
                        rects = mode_attributes[i].second->Detect(depth, seq);
                    
                    // matcher
                    Mat_<double> closenessMapD = createClosenessMap(&prev, &rects, iou_threshold_d);
                    Mat mask(closenessMapD.size(), CV_8UC1, Scalar(255,255,255));            
                    objects.clear();
                    vector<int> taken_prev;
                    vector<int> taken_rects;
                    while(true){
                        double min, max = 0;
                        Point min_loc, max_loc;
                        minMaxLoc(closenessMapD, &min, &max, &min_loc, &max_loc, mask);
                        if( max == 0 ) break;                               
                        ExtendedObjectInfo newone;
                        if( merging_policy == INTERSECTION_MP)
                            newone = prev[max_loc.y] & rects[max_loc.x];                        
                        else
                            newone = prev[max_loc.y] | rects[max_loc.x];                        
                        
                        objects.push_back(newone); 
                        mask.row(max_loc.y).setTo(Scalar(0,0,0));                                    
                        mask.col(max_loc.x).setTo(Scalar(0,0,0)); 
                        taken_prev.push_back(max_loc.y);
                        taken_rects.push_back(max_loc.x);
                    }
                    mask.release();
                    closenessMapD.release();
                    // care about untaken prevs
                    for( size_t j = 0 ; j < prev.size() ; j++ ){
                        if( find(taken_prev.begin(), taken_prev.end(), j) == taken_prev.end() ){
                            // set 0 for currnet attribute
                            prev[j].setScoreWeight(0, mode_attributes[i].second->Weight);
                            objects.push_back(prev[j]);
                        }
                    }
                    // care about untaken rects
                    for( size_t j = 0 ; j < rects.size() ; j++ ){
                        if( find(taken_rects.begin(), taken_rects.end(), j) == taken_rects.end() ){
                            ExtendedObjectInfo tmp = fake_empty_rect & rects[j];
                            objects.push_back(tmp);
                        }
                    }        
                    taken_prev.clear();
                    taken_rects.clear();
                }                                                
            }
            else if( mode_attributes[i].first.first == CHECK ){
                for( size_t j = 0 ; j < objects.size() ; j++ ){
                    bool check_result;
                    if( mode_attributes[i].first.second == RGB)
                        check_result = mode_attributes[i].second->Check2(frame, objects[j]);
                    else if( mode_attributes[i].first.second == DEPTH)
                        check_result = mode_attributes[i].second->Check2(depth, objects[j]);
                    if( ! check_result )
                        objects[j].setScoreWeight(0, mode_attributes[i].second->Weight);
                    else
                        objects[j].setScoreWeight(1, mode_attributes[i].second->Weight); //NOTE TODO KOSTYL this should not be there                    
                }
            }            
            else if(mode_attributes[i].first.first == EXTRACT){
                if( mode_attributes[i].first.second == RGB)
                    mode_attributes[i].second->Extract(frame, &objects);
                else if( mode_attributes[i].first.second == DEPTH)
                    mode_attributes[i].second->Extract(depth, &objects);
            }
            fake_empty_rect.setScoreWeight(0, mode_attributes[i].second->Weight);                                                
        }
            
        // remove low score objects
        auto it = objects.begin();
        while (it != objects.end() ){   
            //it->calcTotalScore();
            it->mergeAllData(merging_policy);
            if( it->total_score < Probability ){
                it = objects.erase(it);
            }
            else
                ++it;
        }       
        // reconvert back to RGB
        if( objects.size() > 0 ){
            if( mode_attributes.back().first.second != RGB ){
                transform_eoi(objects, depth, frame);
            }
        }
        return objects;
    }
      
    
    void SimpleObject::draw(Mat& image, Scalar col  ){
        for (size_t i = 0; i < objects.size(); i++){
            drawOne(image, &objects[i], col);
        }        
    }
    
    void SimpleObject::drawOne(Mat& image, ExtendedObjectInfo* obj, Scalar col){
        // draw basic rectangle, contour and so on            
        obj->draw(image, col);
        
        // add main object info
        string objectInfo = to_string(ID) +": "+ name + " ["+to_string(roundf(obj->total_score * 100) / 100).substr(0,4)+"]"+(obj->track_id == -1 ? "" : "("+to_string(obj->track_id)+")") ;                                    
        Point prevBr = drawFilledRectangleWithText(image, obj->tl(), objectInfo, col);
                
        for(size_t j = 0 ; j < mode_attributes.size() ; j++ ){            
            string symbol;
            if(mode_attributes[j].first.first == DETECT)
                symbol = " + ";
            else if(mode_attributes[j].first.first == CHECK)
                symbol = " - ";
            else
                symbol = " > ";
            
            string attributeName = symbol + mode_attributes[j].second->Name + " ["+(j < obj->scores_with_weights.size() ? to_string(roundf(obj->scores_with_weights[j].first * 100) / 100).substr(0,4) : "-1" )+"]";
            string extracted_info = " ";
            map<string, string>::iterator it;
            for( it  = obj->extracted_info.begin(); it != obj->extracted_info.end(); it++){
                auto res = std::mismatch(mode_attributes[j].second->Name.begin(), mode_attributes[j].second->Name.end(), it->first.begin());
                if( res.first == mode_attributes[j].second->Name.end() ){
                    extracted_info += it->second + " ";
                }
            }
            prevBr = drawFilledRectangleWithText(image, Point(obj->tl().x, prevBr.y), attributeName + extracted_info, col);
            
        }        
   }
   
   void SimpleObject::transform_eoi(std::vector<ExtendedObjectInfo> &rects, const InfoImage& src, const InfoImage& dst){
       if( src.empty() || dst.empty()){
           printf("Error! On of channel data is empty, can't convert!");
           return;
       }
       for( auto& rect : rects){
           // rect
           Point new_tl = transform_between_channels(rect.tl(), src.K(), dst.K());
           Point new_br = transform_between_channels(rect.br(), src.K(), dst.K());
           rect.x = new_tl.x;
           rect.y = new_tl.y;
           rect.width = new_br.x - new_tl.x;
           rect.height = new_br.y - new_tl.y;
           // contour
           for( auto& cc: rect.contour){
               for( auto& c: cc){
                   c = transform_between_channels(c, src.K(), dst.K());
               }
           }                      
           // tvec & rvec - should not be converted as already in 3d
       }
       
   }

}
