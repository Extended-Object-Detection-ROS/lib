/*
Project: Object detection Library
Author: Moscowsky Anton
File: Class realises loading simple_simple_objects parameters from XML file
*/

#ifndef _OBJECT_BASE_
#define _OBJECT_BASE_

#include "tinyxml.h"

#include "ObjectIdentifier.h"

// DETECTORS
#include "HsvColorDetector.h"
#include "HaarCascadeDetector.h"
#include "HoughDetector.h"
#include "SizeDetector.h"

#include "HistColorDetector.h"
#include "DimentionDetector.h"
#include "BasicMotionDetector.h"
#include "ArucoDetector.h"
#include "PoseDetector.h"
#ifdef USE_TF
#include "TensorFlowDetector.h"
#endif
#ifdef USE_OPENCV_CONTRIB
#include "FeatureDetector.h"
#endif
#if (CV_MAJOR_VERSION > 3)
#include "QrDetector.h"
#endif
#if USE_ZBAR
#include "QrZbarDetector.h"
#endif
#include "LogicDetector.h"
#if (CV_MAJOR_VERSION > 3)
#include "DnnDetector.h"
#endif
#include "BlobDetector.h"
#include "DepthDetector.h"
#include "RoughDistanceDetector.h"
#include "DistanceDetector.h"
#if (USE_DLIB)
#include "FaceDlibDetector.h"
#endif
#include "ExtractedInfoIdChecker.h"
#include "ExtractedInfoStringChecker.h"
#include "UnitTranslationExtracter.h"
#include "SquareObjectDistanceDetector.h"
#if (USE_TORCH)
#include "TorchYOLOv7Detector.h"
#endif
#if (USE_ROS)
#include "ROSSubscriberOpenPoseRaw.h"
#endif
#include "KeypointPoseDetector.h"
#include "DummyHumanPoseDetector.h"

#include "Filtering.h"

#include "ComplexObject.h"
#ifdef USE_IGRAPH
#include "ComplexObjectGraph.h"
#endif
// RELATIONS
#include "LogicRelations.h"
#include "ImageRangeRelation.h"
#include "ThreeDimRangeRelation.h"
#include "SpaceRelations.h"
#include "SizeRelations.h"
#include "SameExtractedInfoRelation.h"

#if USE_TRACKING
#include "Tracker.h"
#endif

namespace eod{
                
    class ObjectBase{
    public:
        
        std::vector<Attribute*>  attributes;      
        std::vector<SimpleObject*> simple_objects;
        std::vector<RelationShip*> relations;        
        std::vector<ComplexObject*> complex_objects;
#ifdef USE_IGRAPH        
        std::vector<ComplexObjectGraph*> complex_objects_graph;
#endif
        
        std::map<std::string, void*> inner_storage;

        /// <summary>
        /// Default constructor
        /// </summary>
        ObjectBase();
        
        inline bool isLoaded(){return loaded;}

        void clear();
        void clear_stuff();

        /// <summary>
        /// Loads list of simple_objects from XML file
        /// </summary>
        /// <param name="filename">Path to xml file</param>        
        bool loadFromXML(std::string filename);
        bool loadFromXML(char* filename){return loadFromXML(std::string(filename));}        

        bool loadFromTextData(std::string filedata, std::string path);
        
        /// <summary>
        /// Prints composition of Base
        /// </summary>
        void printContent();

        SimpleObject* getSimpleObjectByID(int id);
        ComplexObject* getComplexObjectByID(int id);
        
#ifdef USE_IGRAPH 
        ComplexObjectGraph* getComplexObjectGraphByID(int id);
#endif

        /// <summary>
        /// Reloaded operator, returns element of object list
        /// </summary>
        /// <param name="index">Index of object</param>
        /// <returns>Object element</returns>
        SimpleObject* & operator[](int index){return simple_objects[index];}
        
        /// <summary>
        /// Returns size object list
        /// </summary>
        /// <returns>Object list size </returns>
        int size(){
            return simple_objects.size();
        }
                
        /// <summary>
        /// Checks the uniqueness of simple_objects ID
        /// </summary>
        /// <returns>True if ID of all simple_objects is unigue, else false </returns>
        bool checkIDobj();
	
        /*
        /// <summary>
        /// Checks the uniqueness of scenes ID
        /// </summary>
        /// <returns>True if ID of all scenes is unigue, else false </returns>
        bool checkIDsc();
        */
        
        /// <summary>
        /// Finds appropriate Attribute type from its name
        /// </summary>
        /// <param name="name">Attribute name</param>
        /// <returns>Attribute type</returns>
        AttributeTypes getTypeFromName(std::string name);
	
        void printDetected();

        // --- some settings
        /*
        bool setStartPattern(std::string objName, pattern pat_start);
        bool addOkPattern(std::string objName, pattern pat_ok);
        */
        
        // camera params for those who want it
        void setCameraParams(cv::Mat cameraMatrix, cv::Mat distCoeffs);
        cv::Mat getCameraMatrix();
        cv::Mat getDistortionCoeff();

        inline std::string getPath(){return object_base_path;}

    private:        
        
        std::string object_base_path;
        
        /// <summary>
        /// Loads list of relationships from XML file
        /// </summary>
        /// <param name="doc">Object base TinyXML class</param>
        bool loadFromXMLr(TiXmlDocument *doc);

        /// <summary>
        /// Loads list of attributes from XML file
        /// </summary>
        /// <param name="doc">Object base TinyXML class</param>
        bool loadFromXMLa(TiXmlDocument *doc, std::string object_base_path);

        /// <summary>
        /// Loads list of simple simple_objects from XML file
        /// </summary>
        /// <param name="doc">Object base TinyXML class</param>
        bool loadFromXMLso(TiXmlDocument *doc);

        
        /// <summary>
        /// Loads list of complex objects from XML file
        /// </summary>
        /// <param name="doc">Object base TinyXML class</param>
        bool loadFromXMLsNM(TiXmlDocument *doc);
        
#ifdef USE_IGRAPH        
        bool loadFromXMLsG(TiXmlDocument *doc);
#endif

        /// <summary>
        /// Gets pointer to object from list by its name
        /// </summary>
        /// <param name="objectname">Name of object</param>
        /// <returns>Pointer to object, or NULL if is not successful</returns>
        SimpleObject* getByName(std::string objectname);
	
        
        // XML read utils
        std::string getPathAttribute(TiXmlElement * attr, const char * at_name);
        std::vector<int> getIntVectorAttribute(TiXmlElement * attr, const char * at_name);
        std::string getStringAttribute(TiXmlElement * attr, const char * at_name, std::string default_value = "");
        std::vector<std::string> getStringVectorAttribute(TiXmlElement * attr, const char * at_name);
        std::vector<double> getDoubleVectorAttribute(TiXmlElement * attr, const char * at_name);
        
        
        ComplexObject* getByNameNS(std::string scenename);
	
        /// <summary>
        /// Gets pointer to Attribute from list by its name
        /// </summary>
        /// <param name="attributename">Name of attribute</param>
        /// <returns>Pointer to attribute, or NULL if is not successful</returns>
        Attribute* getByNameA(std::string attributename);

        /// <summary>
        /// Gets pointer to relationship from list by its name
        /// </summary>
        /// <param name="relname">Name of relation</param>
        /// <returns>Pointer to object, or NULL if is not successful</returns>
        RelationShip* getByNameR(std::string relname);
        
        void readFilters(TiXmlElement* filter_el, std::vector<EoiFilter*> *filters);
        
        bool loaded;        
        cv::Mat cameraMatrix, distCoeffs;
        
        int relation_counter;

    };        
}

#endif
