#ifdef USE_IGRAPH
#include "ComplexObjectGraph.h"
#include "drawing_utils.h"
#include <algorithm>

namespace eod{
        
    
    ComplexObjectGraph::ComplexObjectGraph(){
        graph = Graph();
        identify_mode = HARD;
        Probability = 0.75;
        plot_offset = 3;
    }
    
    void ComplexObjectGraph::add_object(std::string name, SimpleObject* so, int num, double weight){
        int vid = graph.add_vectice(name, so->ID, num, 1, weight);
        ObjectsToGraphsVerticesIds.insert(std::pair<std::string, int>(name, vid));
        ObjectsToSimpleObjects.insert(std::pair<std::string, SimpleObject*>(name, so));
    }
    
    void ComplexObjectGraph::add_relation(std::string o1_name, std::string o2_name, RelationShip* rs, double weight){
        std::string auto_name = std::to_string(graph.get_edges_len());
        
        NamesToRelations.insert(std::pair<std::string, RelationShip*>(auto_name, rs));
        
        NamesToObjects.insert(std::pair<std::string, std::pair<std::string, std::string>>(auto_name,std::pair<std::string, std::string>(o1_name, o2_name)));        
        
        graph.add_edge(auto_name, rs->ID, ObjectsToGraphsVerticesIds[o1_name], ObjectsToGraphsVerticesIds[o2_name], false, weight);
    }
    
    std::vector<ExtendedObjectInfo> ComplexObjectGraph::Identify(const InfoImage& frame, const InfoImage& depth, int seq ){
        // TODO analyze sequence
        if( identify_mode == HARD )
            IdentifyHard(frame, depth, seq);
        else
            IdentifySoft(frame, depth, seq);

        for( size_t i = 0; i < filters.size() ; i ++ ){
            filters[i]->Filter(&complex_objects);
        }
        return complex_objects;
    }
    
    std::vector<ExtendedObjectInfo> ComplexObjectGraph::IdentifyHard(const InfoImage& frame, const InfoImage& depth, int seq ){
        //printf("NEW GRAPH\n");
        std::vector <ExtendedObjectInfo> result;
        
        // FORM A GRAPH
        Graph current_view_graph;
        for(auto const& nto : NamesToObjects){
            //printf("Rel: %s %s\n", nto.first.c_str(), NamesToRelations[nto.first]->Name.c_str());
            std::vector<ExtendedObjectInfo> obj1 = ObjectsToSimpleObjects[nto.second.first]->Identify(frame, depth, seq);
            std::vector<ExtendedObjectInfo> obj2 = ObjectsToSimpleObjects[nto.second.second]->Identify(frame, depth, seq);
            
            for( size_t i = 0 ; i < obj1.size() ; i++ ){
                int ind1 = current_view_graph.add_vectice(nto.second.first, ObjectsToSimpleObjects[nto.second.first]->ID, i, obj1[i].total_score);
                //printf("%i %i -> %i\n",ObjectsToSimpleObjects[nto.second.first]->ID, i, ind1);
                
                for( size_t j = 0 ; j < obj2.size(); j++){
                    
                    int ind2 = current_view_graph.add_vectice(nto.second.second, ObjectsToSimpleObjects[nto.second.second]->ID, j, obj2[j].total_score);
                    //printf("%i %i -> %i\n",ObjectsToSimpleObjects[nto.second.second]->ID, j, ind2);
                    
                    if( NamesToRelations[nto.first]->checkRelation(frame, &obj1[i], &obj2[j]) ){
                        current_view_graph.add_edge(NamesToRelations[nto.first]->Name, NamesToRelations[nto.first]->ID, ind1, ind2);
                    }
                }
            }                        
        }
        
        // DO VF2
        std::vector<std::pair<std::vector<int>, double>> maps = current_view_graph.get_subisomorphisms(&graph);                
        
        // maps:
        // j - vert id graph, maps[_][j] - vert id current_view_graph
        
        // RETRIEVE DATA
        simple_objects.clear();
        for( size_t i = 0 ; i < maps.size() ; i++ ){
            if( maps[i].second < Probability ){
                continue;
            }            
            std::vector<std::pair<std::string, ExtendedObjectInfo*>> simples;
            
            int obj_type, obj_num;
            std::string object_name = current_view_graph.get_vertice_params(maps[i].first[0], &obj_type, &obj_num);
            
            ExtendedObjectInfo merged = ObjectsToSimpleObjects[object_name]->objects[obj_num];
            simples.push_back(std::pair<std::string, ExtendedObjectInfo*>(object_name, &(ObjectsToSimpleObjects[object_name]->objects[obj_num])));
                        
            for( int j = 1 ; j < maps[i].first.size(); j++){
                object_name = current_view_graph.get_vertice_params(maps[i].first[j], &obj_type, &obj_num);
                merged = merged | ObjectsToSimpleObjects[object_name]->objects[obj_num];                
                simples.push_back(std::pair<std::string, ExtendedObjectInfo*>(object_name, &(ObjectsToSimpleObjects[object_name]->objects[obj_num])));
            }
            merged.mergeAllData();
            merged.total_score = maps[i].second;
            result.push_back(merged);
            simple_objects.push_back(simples);
        }
        
        complex_objects = result;
        return result;
    }
    
    std::vector<ExtendedObjectInfo> ComplexObjectGraph::IdentifySoft(const InfoImage& frame, const InfoImage& depth, int seq ){        
        std::vector <ExtendedObjectInfo> result;
        
        //printf("New graph\n");
        // FORM A GRAPH WITH FAKES
        Graph current_view_graph;
        for(auto const& nto : NamesToObjects){
            
            std::vector<ExtendedObjectInfo> obj1 = ObjectsToSimpleObjects[nto.second.first]->Identify(frame, depth, seq);
            std::vector<ExtendedObjectInfo> obj2 = ObjectsToSimpleObjects[nto.second.second]->Identify(frame, depth, seq);                        
            
            for( int i = -1 ; i < (int)obj1.size() ; i++ ){
                //printf("\t i: %i\n", i);
                double dc1 = 0;
                if( i != -1)
                    dc1 = obj1[i].total_score;
                
                int ind1 = current_view_graph.add_vectice(nto.second.first, ObjectsToSimpleObjects[nto.second.first]->ID, i, dc1);  
                
                for( int j = -1 ; j < (int)obj2.size(); j++){
                    //printf("\t j: %i\n", j);
                    double dc2 = 0;
                    if( j != -1)
                        dc2 = obj2[j].total_score;
                    
                    int ind2 = current_view_graph.add_vectice(nto.second.second, ObjectsToSimpleObjects[nto.second.second]->ID, j, dc2);  
                     
                    
                    if( i == -1 || j == -1)
                        current_view_graph.add_edge(NamesToRelations[nto.first]->Name, NamesToRelations[nto.first]->ID, ind1, ind2, true);
                    else{
                        if( NamesToRelations[nto.first]->checkRelation(frame, &obj1[i], &obj2[j]) ){
                            current_view_graph.add_edge(NamesToRelations[nto.first]->Name, NamesToRelations[nto.first]->ID, ind1, ind2, false);
                        }
                        else{
                            current_view_graph.add_edge(NamesToRelations[nto.first]->Name, NamesToRelations[nto.first]->ID, ind1, ind2, true, 0);
                        }
                    }
                }
            }                        
        }        
        // DO VF2
        std::vector<std::pair<std::vector<int>, double>> maps = current_view_graph.get_subisomorphisms(&graph);                        
        // maps:
        // j - vert id graph, maps[_][j] - vert id current_view_graph        
        // RETRIEVE DATA      
        simple_objects.clear();
        for( size_t i = 0 ; i < maps.size() ; i++ ){
            
            if( maps[i].second < Probability ){
                continue;
            }            
            std::vector<std::pair<std::string, ExtendedObjectInfo*>> simples;
            
            int obj_type, obj_num;
            std::string object_name = current_view_graph.get_vertice_params(maps[i].first[0], &obj_type, &obj_num);
            
            ExtendedObjectInfo merged;
                        
            if( obj_num == -1){
            }
            else{
                merged = ObjectsToSimpleObjects[object_name]->objects[obj_num];
                simples.push_back(std::pair<std::string, ExtendedObjectInfo*>(object_name, &(ObjectsToSimpleObjects[object_name]->objects[obj_num])));
            }
                        
            for( size_t j = 1 ; j < maps[i].first.size(); j++){
                object_name = current_view_graph.get_vertice_params(maps[i].first[j], &obj_type, &obj_num);
                if( obj_num == -1){
                }
                else{
                    if( ObjectsToSimpleObjects[object_name]->objects[obj_num].height == 0 or ObjectsToSimpleObjects[object_name]->objects[obj_num].width == 0){
                        //skip
                    }
                    merged = merged | ObjectsToSimpleObjects[object_name]->objects[obj_num];
                    simples.push_back(std::pair<std::string, ExtendedObjectInfo*>(object_name, &(ObjectsToSimpleObjects[object_name]->objects[obj_num])));
                }
            }
            if( merged.x == 0 && merged.y == 0 && merged.width == 0 && merged.height == 0)
                continue;                
            merged.mergeAllData();
            merged.total_score = maps[i].second;
            result.push_back(merged);
            simple_objects.push_back(simples);
        }                        
        //TODO destroy graph
        complex_objects = result;
        
        return result;
    }
        
    void ComplexObjectGraph::drawOne(const cv::Mat& frameTd, int no, cv::Scalar color, int tickness){
        if( no < complex_objects.size() ){
            // NOTE temp
            complex_objects[no].x -= plot_offset;
            complex_objects[no].y -= plot_offset;
            complex_objects[no].height += 2*plot_offset;
            complex_objects[no].width += 2*plot_offset;
            
            complex_objects[no].draw(frameTd, color);
            
            for(auto& n_so : simple_objects[no]){
                n_so.second->draw(frameTd, color);
            }
            
            
            std::string objectInfo = std::to_string(ID)+": "+name +" ["+ std::to_string(complex_objects[no].total_score).substr(0,4)+"]";
            cv::Point prevBr = drawFilledRectangleWithText(frameTd, cv::Point(complex_objects[no].x,complex_objects[no].y /*-12*/)  , objectInfo, color);     
        }
    }
    
    void ComplexObjectGraph::drawAll(const cv::Mat& frameTd, cv::Scalar color, int tickness){
        for( size_t j = 0 ; j < complex_objects.size(); j++ ){
            drawOne(frameTd,j,color,tickness);
        }
    }
}

#endif //USE_IGRAPH
