#include "Graph.h"
#include <stdlib.h>
#include <algorithm>

#ifdef USE_IGRAPH
namespace eod{
    
    std::vector<double> parse_packed_str_d(std::string packed, std::string delimiter = ":"){
        size_t pos = 0;        
        std::vector<double> output;
        while ((pos = packed.find(delimiter)) != std::string::npos) {
            std::string token = packed.substr(0, pos);
            output.push_back(atof(token.c_str()));
            packed.erase(0, pos + delimiter.length());
        }
        if( !packed.empty() )
            output.push_back(atof(packed.c_str()));
        return output;
    }
    
    std::vector<double> parse_packed_str_i(std::string packed, std::string delimiter = ":"){
        size_t pos = 0;        
        std::vector<double> output;
        while ((pos = packed.find(delimiter)) != std::string::npos) {
            std::string token = packed.substr(0, pos);
            output.push_back(atoi(token.c_str()));
            packed.erase(0, pos + delimiter.length());
            //printf("%s-",token.c_str());
        }
        if( !packed.empty() )
            output.push_back(atoi(packed.c_str()));
        return output;
    }
    
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
        // check if it is multi-edge
        bool m1 = EAN(graph1, "multi", e1_num);
        bool m2 = EAN(graph2, "multi", e2_num);
        
        if( !m1 and !m2 )
            return igraph_bool_t(EAN(graph1, "rel_type", e1_num) == EAN(graph2, "rel_type", e2_num) );
        else if(m1 and m2){
            printf("Both edges are multi, you sure?\n");
            return igraph_bool_t(false); // TODO maybe sometimes will be needed
        }
        else{
            if(m1){
                auto types = parse_packed_str_i(EAS(graph1, "rel_type", e1_num));
                int type = EAN(graph2, "rel_type", e2_num);
                bool same = std::find(types.begin(), types.end(), type) != types.end();                
                //printf("Same: %s\n", same ? "true" : "false");
                return igraph_bool_t( same );
            }
            else{
                auto types = parse_packed_str_i(EAS(graph2, "rel_type", e2_num));
                int type = EAN(graph1, "rel_type", e1_num);
                return igraph_bool_t( std::find(types.begin(), types.end(), type) != types.end() );
            }            
        }        
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
        //vertices_colors = other.vertices_colors;
        //edges_colors = other.edges_colors;
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
        
        //vertices_colors.push_back(object_type);
        
        //printf("Added %i vertice\n", vertices_len);
        vertices_len++;        
        return vertices_len-1;
    }
    
    int Graph::add_edge(std::string relation_name, int relation_type, int o1, int o2, bool fake, double weight, double dc){
        //printf("Trying to added edge between vertices %i and %i\n", o1, o2);
        
        igraph_add_edge(&graph, o1, o2);        
        SETEAS(&graph, "rel_name", edges_len, relation_name.c_str());
        SETEAN(&graph, "rel_type", edges_len, relation_type);
        //SETEAB(&graph, "fake", edges_len, igraph_bool_t(fake));
        SETEAN(&graph, "fake", edges_len, int(fake));
        SETEAN(&graph, "weight", edges_len, int(weight*accuracy));
        SETEAN(&graph, "dc", edges_len, int(dc*accuracy));
        //SETEAB(&graph, "multi", edges_len, igraph_bool_t(false));
        SETEAN(&graph, "multi", edges_len, int(false));
        
        //edges_colors.push_back(relation_type);
        
        edges_len++;
        return edges_len;
    }
    
    /*
     * Function that makes an 'multiedge' which is paked in one real edge, to make VF2 work
     */
    void Graph::add_multi_edge(std::string relation_name, int relation_type, int o1, int o2, double weight, double dc){
        // check edge exist
        igraph_vector_t pair;
        igraph_vector_init(&pair, 2);
        VECTOR(pair)[0] = o1;
        VECTOR(pair)[1] = o2;
        igraph_vector_t edge_id;
        igraph_vector_init(&edge_id, 0);        
        igraph_get_eids(&graph, &edge_id, &pair, NULL, 0, 0);
        // add new
        if( VECTOR(edge_id)[0] == -1 ){
            printf("Adding new...\n");
            igraph_add_edge(&graph, o1, o2);            
//             SETEAB(&graph, "multi", edges_len, igraph_bool_t(true));
//             SETEAB(&graph, "fake", edges_len, igraph_bool_t(false));
            SETEAN(&graph, "multi", edges_len, int(true));
            SETEAN(&graph, "fake", edges_len, int(false));
            
            SETEAS(&graph, "rel_name", edges_len, relation_name.c_str());
            SETEAS(&graph, "rel_type", edges_len, std::to_string(relation_type).c_str());
            SETEAS(&graph, "weight", edges_len, std::to_string(weight).substr(0,4).c_str());
            SETEAS(&graph, "dc", edges_len, std::to_string(dc).substr(0,4).c_str());
            
            edges_len++;
        }
        // update existing
        else if(igraph_vector_size(&edge_id) == 1){
            int eid = VECTOR(edge_id)[0];
            printf("There are...\n");
            
            std::string rel_name = EAS(&graph, "rel_name", eid);
            SETEAS(&graph, "rel_name", eid, (rel_name+":"+relation_name).c_str());
            std::string rel_type = EAS(&graph, "rel_type", eid);
            SETEAS(&graph, "rel_type", eid, (rel_type+":"+std::to_string(relation_type)).c_str());
            std::string weight_ = EAS(&graph, "weight", eid);
            SETEAS(&graph, "weight", eid, (weight_+":"+std::to_string(weight).substr(0,4)).c_str());
            std::string dc_ = EAS(&graph, "dc", eid);
            SETEAS(&graph, "dc", eid, (dc_+":"+std::to_string(dc).substr(0,4)).c_str());
            
        }
        else{
            printf("Error! There are more than one relation, despite it already is multiedge!!");
        }                        
    }
    
    /*
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
    */
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
                        int edge_id_ind = VECTOR(edge_id)[0];// takin' first edge only, so it is important for graph to be simple
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
                            double dc_edge_sub = double(EAN(&sub_graph->graph, "dc", edge_id_ind_tar))/accuracy;
                            
                            double dc_edge;
                            
                            if(! EAN(&graph, "multi", edge_id_ind) ){
                                dc_edge = double(EAN(&graph, "dc", edge_id_ind))/accuracy;
                            }
                            else{
                                //dc_edge = 1; // just for test
                                auto indexes = parse_packed_str_i(EAS(&graph, "rel_type", edge_id_ind));
                                auto dcs = parse_packed_str_d(EAS(&graph, "dc", edge_id_ind));
                                
                                int target_type = EAN(&sub_graph->graph, "rel_type", edge_id_ind_tar);
                                
                                auto it = std::find(indexes.begin(), indexes.end(), target_type);
                                if( it != indexes.end() ){
                                    int index = it - indexes.begin();
                                    dc_edge = dcs[index];
                                }
                                else{
                                    printf("Error! Main graph does not contain specifiec enge corresssponing second\n");
                                }
                                
                            }
                                                                                    
                            if( EAN(&graph, "fake", edge_id_ind) ){
                                // IT IS FAKE                                    
                            }
                            else{                                                                
                                // calc
                                Dc += dc_edge*k_edge*(k1 * dc1 + k2 * dc2);                                
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
    
}
#endif // USE_IGRAPH
