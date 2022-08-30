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
        
    private:
        SimpleObject* so;
        ComplexObjectGraph* co;
        double x, y, z, h, r;
        
    };
    
    class Scene{
    public:
        Scene();        
        Scene(std::string name, int id);
        
        void add_object(SceneObject* obj);        
                
        void Identify(const InfoImage& frame, const InfoImage& depth, int seq);
        bool hasClass(std::string class_name);
        
        std::string name;
        int id;        
        std::vector<RelationShip*> relations;
        
    private:
        std::vector<SceneObject*> unique_classes;
        std::vector<SceneObject*> scene_objects;
        
        
        Graph scene_base_graph;
        
    };    
}

#endif // SCENE_H
