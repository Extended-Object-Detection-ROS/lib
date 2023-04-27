#ifndef EOD_GRAPH_H_
#define EOD_GRAPH_H_
#include "igraph.h"
#include <vector>
#include <string>

namespace eod{
    
    /*
     * This class is wrap around igraph's C-style functions
     */
    class Graph{
        
    public:
        Graph(bool directed = true);
        
        Graph(const Graph&);
        
        int add_vectice(std::string object_name, int object_type, int obj_num = 0, double dc = 1, double weight = 1);
        int add_edge(std::string relation_name, int relation_type, int o1, int o2, bool fake = false, double weight = 1, double dc = 1);                
        
        std::vector<std::pair<std::vector<int>, double>> get_subisomorphisms(Graph * sub_graph);
        
        std::string get_vertice_params(int id, int* object_type, int* obj_num);
        
        inline int get_vert_len(){
            return vertices_len;
        }
        
        inline int get_edges_len(){
            return edges_len;
        }
        
        std::string get_color_info();
        
        bool is_simple();
        
    private:
        // graph representation
        igraph_t graph;
        
        // colors, needed for VF2                
        std::vector<int> vertices_colors;
        std::vector<int> edges_colors;
        
        igraph_vector_int_t get_vertices_colors();
        igraph_vector_int_t get_edges_colors();
        
        int vertices_len;
        int edges_len;
        
        int accuracy;                
                
    };
    
}


#endif // EOD_GRAPH_H_
