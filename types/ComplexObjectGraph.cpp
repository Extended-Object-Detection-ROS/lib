#ifdef USE_IGRAPH
#include "ComplexObjectGraph.h"
#include "drawing_utils.h"
#include <algorithm>

namespace eod{
    
    igraph_bool_t compare_vertexes(const igraph_t *graph1,
                                     const igraph_t *graph2,
                                     const igraph_integer_t v1_num,
                                     const igraph_integer_t v2_num,
                                     void *arg){
        
        return igraph_bool_t(VAN(graph1, "obj_type", v1_num) == VAN(graph2, "obj_type", v2_num) );
        
    }
    
    igraph_bool_t compare_edges(const igraph_t *graph1,
                                     const igraph_t *graph2,
                                     const igraph_integer_t e1_num,
                                     const igraph_integer_t e2_num,
                                     void *arg){
        
        return igraph_bool_t(EAN(graph1, "rel_type", e1_num) == EAN(graph2, "rel_type", e2_num) );
        
    }
    
    Graph::Graph(bool directed){
        vertices_len = 0;
        edges_len = 0;
        igraph_empty(&graph, 0, directed ? IGRAPH_DIRECTED : IGRAPH_UNDIRECTED);
        accuracy = 100;
    }        
        
    Graph::Graph(const Graph& other){
        vertices_len = other.vertices_len;
        edges_len = other.edges_len;
        igraph_copy(&graph, &(other.graph));
        accuracy = other.accuracy;
        vertices_colors = other.vertices_colors;
        edges_colors = other.edges_colors;
    }
    
    /*
     * object_name - inner name
     * object_type - unique id of type
     * obj_num - index of object in list to find it back
     */
    int Graph::add_vectice(std::string object_name, int object_type, int obj_num, double dc, double weight){
        // check this already 
        if( vertices_len > 0 ){            
            igraph_vector_t types;
            igraph_vector_t nums;            
            igraph_vector_init (&types, 0);
            igraph_vector_init (&nums, 0);            
            VANV(&graph, "obj_type", &types);
            VANV(&graph, "obj_num", &nums);
                        
            //long int ind_type, ind_num;
            // TODO if check that it exists will be faster
            for( int i = 0 ; i < vertices_len ; i++){
                if( object_type == VECTOR(types)[i] && obj_num == VECTOR(nums)[i] && object_name == VAS(&graph, "obj_name", i) ){
                    printf("Vertice %i already added\n", i);
                    return i; //NOTE true?
                }
            }            
        }
                
        igraph_add_vertices( &graph, 1, NULL ); 
        SETVAS(&graph, "obj_name", vertices_len, object_name.c_str());
        SETVAN(&graph, "obj_type", vertices_len, object_type);
        SETVAN(&graph, "obj_num", vertices_len, obj_num);
        SETVAN(&graph, "dc", vertices_len, int(dc*accuracy));
        SETVAN(&graph, "weight", vertices_len, int(weight*accuracy));
        
        vertices_colors.push_back(object_type);
        
        //printf("Added %i vertice\n", vertices_len);
        vertices_len++;        
        return vertices_len-1;
    }
    
    int Graph::add_edge(std::string relation_name, int relation_type, int o1, int o2, bool fake, double weight, double dc){
        //printf("Trying to added edge between vertices %i and %i\n", o1, o2);
        
        igraph_add_edge(&graph, o1, o2);
        SETEAS(&graph, "rel_name", edges_len, relation_name.c_str());
        SETEAN(&graph, "rel_type", edges_len, relation_type);
        SETEAN(&graph, "fake", edges_len, int(fake));
        SETEAN(&graph, "weight", edges_len, int(weight*accuracy));
        SETEAN(&graph, "dc", edges_len, int(dc*accuracy));
        
        edges_colors.push_back(relation_type);
        
        edges_len++;
        return edges_len;
    }
    
    /* Not working function
    // If such color (type) edge is already in graph, it will be replaced IF current dc is higher
    int Graph::add_edge_replace(std::string relation_name, int relation_type, int o1, int o2, bool fake, double weight, double dc){
        printf("Adding replace \n");
        //if( edges_len == 0 )
        if( std::find(edges_colors.begin(), edges_colors.end(), relation_type) == edges_colors.end() )
            return add_edge(relation_name, relation_type, o1, o2, fake, weight, dc);
        
        igraph_es_t es; // edge selector
        igraph_eit_t eit; // edge iterator
        
        //igraph_es_pairs_small(&es, IGRAPH_DIRECTED, o1, o2, -1); // create selector between two vert
        //igraph_es_all(&es, IGRAPH_EDGEORDER_ID);
        //igraph_vector_t v;
        //igraph_vector_init(&v, 2);
        //VECTOR(v)[0] = o1;
        //VECTOR(v)[1] = o2;
        //igraph_es_pairs(&es, &v, IGRAPH_DIRECTED);
        igraph_es_incident(&es, o1, IGRAPH_ALL);
        printf("&&&\n");
        igraph_eit_create(&graph, es, &eit); // create iterator
        printf("&&&\n");
        
        int size_edges = IGRAPH_EIT_SIZE(eit);
        printf("Size enges %i\n",size_edges);
        if( size_edges == 0 ){
            igraph_eit_destroy(&eit);
            igraph_es_destroy(&es);
            return add_edge(relation_name, relation_type, o1, o2, fake, weight, dc);
        }        
        while( !IGRAPH_EIT_END(eit) ){
            int edge_id = IGRAPH_EIT_GET(eit);
            // find same type
            if( EAN(&graph, "rel_type", edge_id) == relation_type ){
                if( EAN(&graph, "dc", edge_id) < int(dc*accuracy) ){
                    SETEAN(&graph, "weight", edges_len, int(weight*accuracy));
                    SETEAN(&graph, "dc", edges_len, int(dc*accuracy));
                    
                    igraph_eit_destroy(&eit);
                    igraph_es_destroy(&es);
                    return edges_len;
                }
            }
            IGRAPH_VIT_NEXT(eit);
        }

        igraph_eit_destroy(&eit);
        igraph_es_destroy(&es);
        return edges_len;
    }
    */
    
    igraph_vector_int_t Graph::get_vertices_colors(){
        igraph_vector_int_t vc;
        igraph_vector_int_init(&vc, vertices_len);
        
        for( int i = 0 ; i < vertices_len ; i++ )
            VECTOR(vc)[i] = vertices_colors[i];
        return vc;
    }
    
    igraph_vector_int_t Graph::get_edges_colors(){
        igraph_vector_int_t ec;
        igraph_vector_int_init(&ec, edges_len);
        
        for( int i = 0 ; i < edges_len ; i++ )
            VECTOR(ec)[i] = edges_colors[i];
        //printf("Edges len %i\n",edges_len);
        return ec;
    }
    
    std::string Graph::get_color_info(){
        std::string info = "Vert:";
        for(auto& c : vertices_colors){
            info+=(std::to_string(c)+"-");
        }
        info+="\nEdge:";
        for(auto& e : edges_colors){
            info+=(std::to_string(e)+"-");
        }
        return info;
    }
    
    /*
     * Returns:
     * vector of sub isomorphisms:
     *  pair of:
     *      vector of ids of main which corresponds to sub
     *      score of isomorphism
     *  
     */
    std::vector<std::pair<std::vector<int>, double>> Graph::get_subisomorphisms(Graph * sub_graph){
        
        igraph_vector_ptr_t maps;
        igraph_vector_ptr_init(&maps, 0);
        
        /*
        igraph_vector_int_t vert1 = this->get_vertices_colors();
        igraph_vector_int_t vert2 = sub_graph->get_vertices_colors();
        igraph_vector_int_t edg1 = this->get_edges_colors();
        igraph_vector_int_t edg2 = sub_graph->get_edges_colors();
        igraph_get_subisomorphisms_vf2(&graph, &(sub_graph->graph), &vert1, &vert2, &edg1, &edg2, &maps, 0, 0, 0);
        */
        // by adding compare functions there is no need to colorize graph
        igraph_get_subisomorphisms_vf2(&graph, &(sub_graph->graph), 0, 0, 0, 0, &maps, compare_vertexes, compare_edges, 0);
        
        std::vector<std::pair<std::vector<int>, double>> vect_maps;
        
        int n_subis = igraph_vector_ptr_size(&maps);
        for(size_t i = 0; i < n_subis; i++ ){
            igraph_vector_t *temp = (igraph_vector_t*) VECTOR(maps)[i];
            std::vector<int> map;
            
            int n_map = igraph_vector_size(temp);
            for(size_t j = 0; j < n_map; j++){
                // j of graph2 --> VECTOR(*temp)[j] of graph1;
                map.push_back(VECTOR(*temp)[j]);
            }
            vect_maps.push_back(std::pair<std::vector<int>,double>(map, 0));      
            igraph_vector_destroy(temp);
            igraph_free(temp);
        }
        igraph_vector_ptr_destroy(&maps);       
        
        // get Dc
        //std::vector<double> Dcs;
        
        igraph_vector_t pair;
        igraph_vector_init(&pair, 2);
        igraph_vector_t edge_id;
        igraph_vector_init(&edge_id, 0);
        
        for( size_t i = 0 ; i < vect_maps.size() ; i++ ){
            double Dc = 0;
            //int edges_cnt = 0;
            double denominator = 0; // TODO calc on graph init
            for( size_t j1 = 0; j1 < vect_maps[i].first.size(); j1++ ){
                for( size_t j2 = j1+1; j2 < vect_maps[i].first.size(); j2++ ){
                    //if( j1 != j2 ){
                        VECTOR(pair)[0] = vect_maps[i].first[j1];
                        VECTOR(pair)[1] = vect_maps[i].first[j2];
                        igraph_get_eids(&graph, &edge_id, &pair, NULL, 0, 0);
                        int edge_id_ind = VECTOR(edge_id)[0];
                        if( edge_id_ind != -1){                        
                            double dc1 = double(VAN(&graph, "dc", vect_maps[i].first[j1]))/accuracy;
                            double dc2 = double(VAN(&graph, "dc", vect_maps[i].first[j2]))/accuracy;
                            double k1 = double(VAN(&(sub_graph->graph), "weight", j1))/accuracy;
                            double k2 = double(VAN(&(sub_graph->graph), "weight", j2))/accuracy;
                            
                            // extract edge w from target graph
                            VECTOR(pair)[0] = j1;
                            VECTOR(pair)[1] = j2;
                            igraph_get_eids(&(sub_graph -> graph), &edge_id, &pair, NULL, 0, 0);
                            int edge_id_ind_tar = VECTOR(edge_id)[0];
                            double k_edge = double(EAN(&(sub_graph->graph), "weight", edge_id_ind_tar))/accuracy;
                            double dc_edge = double(EAN(&graph, "dc", edge_id_ind))/accuracy;
                            double dc_edge_sub = double(EAN(&sub_graph->graph, "dc", edge_id_ind_tar))/accuracy;
                            
                            
                            if( EAN(&graph, "fake", edge_id_ind) ){
                                // IT IS FAKE                                    
                            }
                            else{                                                                
                                // calc
                                Dc += dc_edge_sub*dc_edge*k_edge*(k1 * dc1 + k2 * dc2);                                
                            }   
                            denominator += k_edge*(k1 + k2);
                        }
                }
            }                        
            Dc /= denominator;                      
            vect_maps[i].second = Dc;
        }        
        return vect_maps;                
    }
    
    std::string Graph::get_vertice_params(int id, int* object_type, int* obj_num){
        *object_type = VAN(&graph, "obj_type", id);
        *obj_num = VAN(&graph, "obj_num", id);
        return std::string(VAS(&graph, "obj_name", id));        
    }
    
    bool Graph::is_simple(){
        igraph_bool_t res;        
        igraph_is_simple(&graph, &res);
        return (bool)res;
    }
            
    //----------------------
    // COMPLEX
    //----------------------
    
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
