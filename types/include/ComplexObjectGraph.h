#ifndef COMPLEX_OBJECT_GRAPH_H_
#define COMPLEX_OBJECT_GRAPH_H_

#include "SimpleObject.h"
#include "Relationship.h"
#include "Filtering.h"
#include "Graph.h"

namespace eod{
            
    class ComplexObjectGraph{
        
    public:
        ComplexObjectGraph();
                
        std::string name;
        int ID;                
        IdentifyMode identify_mode;  
        
        Graph graph;
        double degree_of_confidence;
        
        double Probability;
        int plot_offset;
        
        void add_object(std::string name, SimpleObject* so, int num = 0, double weight = 1);
        void add_relation(std::string o1_name, std::string o2_name, RelationShip* rs, double weight = 1);
        
        std::vector<ExtendedObjectInfo> Identify(const InfoImage& frame, const InfoImage& depth, int seq);
        
        std::vector<ExtendedObjectInfo> IdentifyHard(const InfoImage& frame, const InfoImage& depth, int seq);
        std::vector<ExtendedObjectInfo> IdentifySoft(const InfoImage& frame, const InfoImage& depth, int seq);
        
        void drawOne(const cv::Mat& frameTD, int no, cv::Scalar color, int tickness);
        void drawAll(const cv::Mat& frameTD, cv::Scalar color, int tickness);
        
        std::vector<ExtendedObjectInfo> complex_objects;
        std::vector<std::vector<std::pair<std::string, ExtendedObjectInfo*>>> simple_objects;

        std::vector<EoiFilter*> filters;
                        
    private:
        std::map<std::string, int> ObjectsToGraphsVerticesIds;
        std::map<std::string, SimpleObject*> ObjectsToSimpleObjects;
        
        std::map<std::string, RelationShip*> NamesToRelations;
        //std::map<std::string, double> NamesToRelationsWights;

        std::map<std::string, std::pair<std::string, std::string>> NamesToObjects;
        
        
    };
    
}

#endif //COMPLEX_OBJECT_GRAPH_H_
