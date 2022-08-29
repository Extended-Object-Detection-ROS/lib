#include "Scene.h"

namespace eod{
    
    SceneObject::SceneObject(std::string name_, double x_, double y_, double z_, double h_, double r_){
        name = name_;        
        so = NULL;
        co = NULL;
        x = x_;
        y = y_;
        z = z_;
        r = r_;
        h = h_;
    }
    
    SceneObject::SceneObject(std::string name, double x, double y, double z, double h, double r, SimpleObject* so_) : SceneObject(name, x, y, z, h, r){
        so = so_;
    }
    
    SceneObject::SceneObject(std::string name, double x, double y, double z, double h, double r, ComplexObjectGraph* co_) : SceneObject(name, x, y, z, h, r){
        co = co_;
    }
    
    int SceneObject::ID(){
        if(so)
            return so->ID;
        else
            return co->ID;
    }
    
    std::string SceneObject::class_name(){
        if(so)
            return so->name;
        else
            return co->name;
    }
    
    std::vector<ExtendedObjectInfo> SceneObject::Identify(const InfoImage& frame, const InfoImage& depth, int seq){
        
        if( so == NULL and co == NULL){
            printf("SceneObject %s not initialized!\n",name.c_str());
            return std::vector<ExtendedObjectInfo>();
        }
        else if( co == NULL){
            return so->Identify(frame, depth, seq);
        }
        else{
            return co->Identify(frame, depth, seq);
        }
    }
    
    Scene::Scene(){       
    }
    
    Scene::Scene(std::string name_, int id_){
        name = name_;
        id = id_;                
    }    
    
    void Scene::add_object(SceneObject obj){
        if(!hasClass(obj.class_name())){
            unique_classes.push_back(&obj);
        }        
        
        scene_objects.push_back(obj);
        int id = obj.ID(); // DANGER: when complex objects will be used in scenes, we must check that its ids doesnot intercect with simples
        scene_base_graph.add_vectice(obj.name, id, scene_objects.size(), 1, 1);                        
    }
    
    bool Scene::hasClass(std::string class_name){
        for( auto& obj : scene_objects){
            if( obj.class_name() == class_name){
                return true;
            }
        }
        return false;
    }
    
    void Scene::Identify(const InfoImage& frame, const InfoImage& depth, int seq){        
        Graph observing_scene_graph;       
        // 1. form observing scene graph
        // // 1.1. detecting objects of observing scene
        std::vector<ExtendedObjectInfo> every_detections;
        for( auto& scene_obj : unique_classes){            
            std::vector<ExtendedObjectInfo> res = scene_obj->Identify(frame, depth, seq);
            every_detections.insert(every_detections.end(), res.begin(), res.end());            
            // // // 1.1.1 add vectices            
            for( size_t i = 0 ;i < res.size(); i++){
                observing_scene_graph.add_vectice(scene_obj->class_name(), scene_obj->ID(), i, res[i].total_score, 1);
            }
        }
        if( every_detections.size() == 0 ){
            return;
        }
        // // 1.2. define relations on observing scene        
        for( size_t i = 0 ; i < every_detections.size() ; i++ ){
            for( size_t j = 0 ; j < every_detections.size() ; j++ ){
                if( i != j ){
                    for( auto& rel : relations ){
                        rel->extractParams(frame, &every_detections[i], &every_detections[j]);
                        // TODO here need to copy extracted parameters and store it somehow
                    }
                    
                }
            }
        }        
        
        // 3. compose main scene by observing scene objects and relations
        
        
        // 4. find subisomorphism of observing to main
        
    }
}
