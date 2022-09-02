#ifndef SCENE_H
#define SCENE_H

#include "ComplexObjectGraph.h"

namespace eod{
    
    class SceneObject{
    public:
        SceneObject(std::string name, double x, double y, double z, double h, double r);
        SceneObject(std::string name, double x, double y, double z, double h, double r, SimpleObject* so);
        SceneObject(std::string name, double x, double y, double z, double h, double r, ComplexObjectGraph* co);
        int ID();
        std::string class_name();
        
        std::string name;        
        std::vector<ExtendedObjectInfo> Identify(const InfoImage& frame, const InfoImage& depth, int seq);
        
        ExtendedObjectInfo eoi;
        
        double x, y, z, h, r;  
        
        std::vector<ExtendedObjectInfo>* results();
        
    private:
        SimpleObject* so;
        ComplexObjectGraph* co;
              
    };
    
    class RegisteredRelation{
    public:
        RegisteredRelation(RelationShip* rel, std::string o1, std::string o2, double thresh){
            relation = rel;
            object_class1 = o1;
            object_class2 = o2;
            threshold = thresh;
        }
        RelationShip* relation;
        std::string object_class1;
        std::string object_class2;
        double threshold;
    };
    
    /*
    class OutputScene{
    public:
        double score;
        std::vector<std::pair<SceneObject*, ExtendedObjectInfo*>>;
    private:
    };*/
    
    class Scene{
    public:
        Scene();        
        Scene(std::string name, int id, double prob);
        
        void add_object(SceneObject* obj);        
                
        void Identify(const InfoImage& frame, const InfoImage& depth, int seq);
        bool hasClass(std::string class_name);
        
        void add_relation(RelationShip* rel, double threshold);
        
        std::string name;
        int id;
        
        std::vector<std::pair<double, std::vector<std::pair<SceneObject*, ExtendedObjectInfo*>>>> results;
        
        std::vector<SceneObject*> scene_objects;
        
    private:
        std::vector<SceneObject*> unique_classes;
        
        std::vector<std::pair<RelationShip*, double>> relations;
        
        Graph scene_base_graph;
        
        double probability;
        
    };    
}

#endif // SCENE_H
