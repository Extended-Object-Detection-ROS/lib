#include "Scene.h"
#include "geometry_utils.h"
#include "math_utils.h"

namespace eod{
        
    
    double compare_sizes(SceneObject* sc_obj, ExtendedObjectInfo* vis_obj, const InfoImage& info_im_, double sigma_percent_, double p_aligned_){
        
        cv::Vec3d tl = get_translation(vis_obj->tl(), info_im_.K(), vis_obj->tvec[0][2]);
        cv::Vec3d br = get_translation(vis_obj->br(), info_im_.K(), vis_obj->tvec[0][2]);
        
        double vis_h = br[1] - tl[1];
        double vis_w = br[0] - tl[0];
                        
        bool h_aligned = (vis_obj->x <= 0) || (vis_obj->x + vis_obj->height >= info_im_.height());
        bool w_aligned = (vis_obj->y <= 0) || (vis_obj->y + vis_obj->width >= info_im_.width());
                       
        double sc_obj_w = sc_obj->r*2;
        
        double max_h = std::max(vis_h, sc_obj->h);
        double max_w = std::max(vis_w, sc_obj_w);
        //double min_h = std::min(vis_h, sc_obj->h);
        //double min_w = std::min(vis_w, sc_obj_w);
                
        double p_h, p_w;
        p_h = norm_distribution(vis_h, sc_obj->h, max_h * sigma_percent_);
        p_w = norm_distribution(vis_w, sc_obj_w, max_w * sigma_percent_);
        
        if( h_aligned ){
            if( sc_obj->h > vis_h ){
                p_h = std::max(p_aligned_, p_h);
            }            
        }        
        if( w_aligned ){
            if( sc_obj_w > vis_w ){
                p_w = std::max(p_aligned_, p_w);
            }            
        }                
        return p_h * p_w;
    }
    
    // SceneObject
    
    SceneObject::SceneObject(std::string name_, double x_, double y_, double z_, double h_, double r_){
        name = name_;        
        so = NULL;
        co = NULL;
        x = x_;
        y = y_;
        z = z_;
        r = r_;
        h = h_;
        
        eoi.tvec.push_back(cv::Vec3d(x, y, z));
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
    
    std::vector<ExtendedObjectInfo>* SceneObject::results(){        
        if( so == NULL and co == NULL){
            printf("SceneObject %s not initialized!\n",name.c_str());
            return NULL;
        }
        else if( co == NULL){
            return &(so->objects);
            
        }
        else{
            return &(co->complex_objects); // TODO
        }
    }
    
    Scene::Scene(){       
    }
    
    Scene::Scene(std::string name_, int id_, double prob, std::string frame_id_){
        name = name_;
        id = id_;     
        probability = prob;
        scene_base_graph = Graph(false);
        frame_id = frame_id_;
    }    
    
    void Scene::add_object(SceneObject* obj){                
        scene_objects.push_back(obj);
        if(!hasClass(obj->class_name()))       
            unique_classes.push_back(obj);    
        
        int id = obj->ID(); // DANGER: when complex objects will be used in scenes, we must check that its ids doesnot intercect with simples
        scene_base_graph.add_vectice(obj->name, id, scene_objects.size()-1, 1, 1);                        
    }
    
    void Scene::add_relation(RelationShip* rel, double threshold_create, double threshold_match){
        relations.push_back(std::make_pair(rel, std::make_pair(threshold_create,threshold_match)));
    }
    
    bool Scene::hasClass(std::string class_name){
        for( auto& obj : unique_classes){
            if( obj->class_name() == class_name)
                return true;
        }
        return false;
    }
    
    void Scene::Identify(const InfoImage& frame, const InfoImage& depth, int seq){        
        results.clear();
        Graph observing_scene_graph(false);       
        std::vector<RegisteredRelation> new_relations;
        
        //printf("1\n");
        // 1. form observing scene graph
        // // 1.1. detecting objects of observing scene
        std::vector<ExtendedObjectInfo*> every_detections;
        std::vector<std::string> classes;
        for( auto& scene_obj : unique_classes){            
                        
            scene_obj->Identify(frame, depth, seq);
            printf("Identifying <%s> : %i\n",scene_obj->class_name().c_str(), scene_obj->results()->size());
            
            for(size_t i = 0 ; i < scene_obj->results()->size() ; i++ ){
                ExtendedObjectInfo* eoi = &(scene_obj->results()->at(i));
                
                if( eoi->tvec.size() > 0){
                    every_detections.push_back(eoi);
                    classes.push_back(scene_obj->class_name());
                
                    observing_scene_graph.add_vectice(scene_obj->class_name(), scene_obj->ID(), i, eoi->total_score, 1);
                }
            }            
        }
        printf("%i every_detections\n",every_detections.size());          
        if( every_detections.size() == 0 ){            
            return;
        }
        
        
        // // 1.2. define relations on observing scene        
        defineRelationsOneByone(frame, new_relations, every_detections, observing_scene_graph, classes);        
        printf("New relations size %i\n",new_relations.size());
        if( new_relations.size() == 0 )
            return;
        
        
        // 2. compose main scene by observing scene objects and relations        
        // NOTE: maybe better also to delete extra object from main scene (which is not presented on observing one) - it should be done, because it will reduce relation checking step
        Graph main_scene = Graph(scene_base_graph);   
        //printf("graph: %i %i\n", main_scene.get_vert_len(), main_scene.get_edges_len());
        
        for( size_t i = 0 ; i < scene_objects.size() ; i++ ){                
            for( size_t j = i+1 ; j < scene_objects.size() ; j++ ){
                //double max_score = 0;
                //int best_rel = 0;
                for( size_t k = 0 ; k < new_relations.size(); k++){                    
                    if( scene_objects[i]->class_name() != new_relations[k].object_class1 )
                        continue;
                    if( scene_objects[j]->class_name() != new_relations[k].object_class2 )
                        continue;
                    RelationShip* rel = new_relations[k].relation;
                                        
                    double score = rel->checkSoft(frame, &(scene_objects[i]->eoi), &(scene_objects[j]->eoi));
                    /*
                    if( score > max_score ){
                        max_score = score;
                        best_rel = k;
                    }*/
                                        
                //}
                    //RelationShip* rel = new_relations[best_rel].relation;
                    if( score > new_relations[k].threshold_match){ 
                        main_scene.add_multi_edge(rel->Name, k, i, j, 1, score);
                        printf("Added base [%s] ---%i,%s,%f--- [%s]\n",scene_objects[i]->name.c_str(), k, rel->Name.c_str(), score, scene_objects[j]->name.c_str());
                    }
                }
            }
        }        
        // 3. find subisomorphism of observing to main
                
        
        // 3.1. CALC ADDITIONAL WEIGHTS
        cv::Mat_<double> scores;
        if( use_size_sim ){
            scores = cv::Mat_<double>(scene_objects.size(), every_detections.size());
            for( size_t i = 0 ; i < scene_objects.size() ; i++ ){
                for( size_t j = 0 ; j < every_detections.size() ; j++ ){
                    scores[i][j] = compare_sizes(scene_objects[i], every_detections[j], frame, sigma_pc, aligned_p);
                }
            }
        }
                
        // 3.2. Get Isomorphisms
        std::vector<std::pair<std::vector<int>, double>> maps = main_scene.get_subisomorphisms(&observing_scene_graph, scores);                
        
        // 4. get data        
        //printf("4\n");
        /* scenes:
         * vector of pairs:
         * -- pair of
         * ---- score
         * ---- vector of pairs:
         * ------ pointer to scene object
         * ------ pointer to EOI
         */
                
        printf("observing_scene_graph: %i, %i\n",observing_scene_graph.get_vert_len(), observing_scene_graph.get_edges_len());
        printf("main_scene: %i %i\n",main_scene.get_vert_len(), main_scene.get_edges_len());
        //printf("obs: %s\n",observing_scene_graph.get_color_info().c_str());
        //printf("main: %s\n",main_scene.get_color_info().c_str());
        printf("Isomorphisms size: %i\n",maps.size());
                
        printf("obs: %s main: %s\n",observing_scene_graph.is_simple() ? "simple" : "notsimple", main_scene.is_simple() ? "simple" : "notsimple");
                
        for( size_t i = 0 ; i < maps.size() ; i++ ){            
            std::pair<double,std::vector<std::pair<SceneObject*, ExtendedObjectInfo*>>> scene;
            scene.first = maps[i].second;           
            if( scene.first < probability )
                continue;
            printf("Scene %i %f\n", i, scene.first);
            for( size_t j = 0 ; j < maps[i].first.size() ; j++ ){
                // j - no of vectice in observing_scene_graph
                // maps[i].first[j] - no of vectice in main_scene
                std::pair<SceneObject*, ExtendedObjectInfo*> obj_pair;
                obj_pair.first = scene_objects[maps[i].first[j]];
                obj_pair.second = every_detections[j];
                printf("\t[%i] --> [%i] (%s)\n",j, maps[i].first[j], obj_pair.first->name.c_str());
                
                // try calc score here
                //double dc_object = obj_pair.second.total_score;

                scene.second.push_back(obj_pair);
            }
            results.push_back(scene);
        }  
        printf("~~~~~~~~~~~Scenes done~~~~~~~~~~~~~\n");
        //return results;
    }
        
    
    void Scene::defineRelationTogether(const InfoImage& frame, std::vector<RegisteredRelation>& new_relations, const std::vector<ExtendedObjectInfo*>& every_detections, Graph& observing_scene_graph, const std::vector<std::string>& classes){
        // PLAN
        // get all distances
        
    }    
    
    void Scene::defineRelationsOneByone(const InfoImage& frame, std::vector<RegisteredRelation>& new_relations, const std::vector<ExtendedObjectInfo*>& every_detections, Graph& observing_scene_graph, const std::vector<std::string>& classes){
        for( size_t i = 0 ; i < every_detections.size() ; i++ ){
            for( size_t j = i+1 ; j < every_detections.size() ; j++ ){                
                for( auto& rel : relations ){                        
                    // NOTE: now I try to store all relations, but maybe in future it will better to compare new extracted relation with previous ones (maybe not)
                    bool find_appr = false;
                    double max_score = 0;
                    int best_rel = -1;
                    for( size_t k = 0 ; k < new_relations.size(); k++){
                        auto new_rel = new_relations[k];
                        // check type and objects it operates
                        if( new_rel.relation->Name == rel.first->Name && new_rel.object_class1 == classes[i] && new_rel.object_class2 == classes[j] ){                                
                            double new_score = new_rel.relation->checkSoft(frame, every_detections[i], every_detections[j]);
                            if( new_score > new_rel.threshold_create ){
                                if( new_score > max_score ){
                                    max_score = new_score;
                                    best_rel = k;
                                }
                                
                                //observing_scene_graph.add_edge(rel.first->Name, k, i, j, false, 1, score);
                                find_appr = true;                                    
                                //break;
                            }                                
                        }
                    }                        
                    if( find_appr ){
                        observing_scene_graph.add_edge(rel.first->Name, best_rel, i, j, false, 1, max_score);
                        printf("Existing relation used (%s %s [%.2f]) between %s and %s\n", rel.first->Name.c_str(), rel.first->params_as_str().c_str(), max_score, classes[i].c_str(), classes[j].c_str());
                        continue;
                    }
                    
                    if( rel.first->extractParams(frame, every_detections[i], every_detections[j])){
                        printf("New relation %i (%s %s) between %s and %s \n",new_relations.size(), rel.first->Name.c_str(), rel.first->params_as_str().c_str() , classes[i].c_str(), classes[j].c_str());
                        // copy extracted parameters and store it                        
                        new_relations.push_back(RegisteredRelation(rel.first->copy(), classes[i], classes[j], rel.second));
                        // also add relations on observing scene
                        observing_scene_graph.add_edge(rel.first->Name, new_relations.size()-1, i, j, false, 1);
                    }
                }
            }
        }
    }
    
}
