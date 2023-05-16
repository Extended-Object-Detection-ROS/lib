#ifndef SCENE_H
#define SCENE_H

#include "Graph.h"
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
    
    
    class SceneComparation{
    public:
        SceneComparation(){}        
        virtual double compare(SceneObject* scene_obj, ExtendedObjectInfo* vis_obj) = 0;
    private:        
    };
    
    
    class SizeSceneComparation : public SceneComparation{
    public:
        SizeSceneComparation(const InfoImage& info_im, double sigma_percent, double p_aligned = 0.5);
        double compare(SceneObject* scene_obj, ExtendedObjectInfo* vis_obj);
    private:
        InfoImage info_im_;
        double sigma_percent_;
        double p_aligned_;
    };
    
    
    class RegisteredRelation{
    public:
        RegisteredRelation(RelationShip* rel, std::string o1, std::string o2, std::pair<double, double> thresh){
            relation = rel;
            object_class1 = o1;
            object_class2 = o2;
            threshold_create = thresh.first;
            threshold_match = thresh.second;
        }
        RelationShip* relation;
        std::string object_class1;
        std::string object_class2;
        double threshold_create;
        double threshold_match;
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
        Scene(std::string name, int id, double prob, std::string frame_id);
        
        void add_object(SceneObject* obj);        
                
        void Identify(const InfoImage& frame, const InfoImage& depth, int seq);
        bool hasClass(std::string class_name);
        
        void add_relation(RelationShip* rel, double threshold_create, double threshold_match);
        
        std::string name;
        int id;
        std::string frame_id;
        
        std::vector<std::pair<double, std::vector<std::pair<SceneObject*, ExtendedObjectInfo*>>>> results;
        
        std::vector<SceneObject*> scene_objects;
        
    private:
        std::vector<SceneObject*> unique_classes;
        
        std::vector<std::pair<RelationShip*, std::pair<double, double>>> relations;
        
        Graph scene_base_graph;
        
        double probability;
        
        void defineRelationsOneByone(const InfoImage& frame, std::vector<RegisteredRelation>& new_relations, const std::vector<ExtendedObjectInfo*>& every_detections, Graph& observing_scene_graph, const std::vector<std::string>& classes);
        
        void defineRelationTogether(const InfoImage& frame, std::vector<RegisteredRelation>& new_relations, const std::vector<ExtendedObjectInfo*>& every_detections, Graph& observing_scene_graph, const std::vector<std::string>& classes);
        
    };    
}

#endif // SCENE_H
