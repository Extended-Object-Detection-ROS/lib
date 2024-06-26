/*
Project: Object detection Library
Author: Moscowsky Anton
File: Attribute description
*/
#ifndef _ATTRIBUTE_
#define _ATTRIBUTE_

#include "ExtendedObjectInfo.h"
#include "Filtering.h"
#include "Clusterization.h"
#include "InfoImage.h"

namespace eod{
    
    class ObjectBase;
    
    enum AttributeMode {
        DETECT,
        CHECK,
        EXTRACT
    };
            
    enum AttributeChannel {
        RGB,
        DEPTH
    };
    
    enum AttributeTypes {
        UNK_A = 0,
        HSV_COLOR_A,
        HAAR_CASCADE_A,
        SIZE_A,        
        HIST_COLOR_A,
        HOUGH_A,
        DIMEN_A,
        BASIC_MOTION_A,
        ARUCO_A,
        FEATURE_A,
        POSE_A,
        TF_A,
        DNN_A,
        QR_A,
        QR_ZBAR_A,
        LOG_AND_A,
        LOG_NOT_A,
        LOG_OR_A,
        LOG_XOR_A,
        BLOB_A,
        DEPTH_A,
        ROUGH_DIST_A,
        DIST_A,
        FACE_DLIB_A,
        EI_ID_CHECK_A,
        EI_STR_CHECK_A,
        UNIT_TRANS_EXTR_A,
        SQUARE_OBJ_DIST_EXTR_A,
        TORCH_YOLOV7_A,
        TORCH_YOLOV7_KPT_A,
        ROS_SUB_BASE_A,
        ROS_SUB_OPENPOSE_RAW_A,
        KPT_POSE_A,
        DUMMY_POSE_A,
    };
    
    AttributeTypes getAttributeTypeFromName(std::string name);
    
    class Attribute{
        public:
            Attribute();                        

            int Type;
            int ID;
            std::string Name;     
            double Weight;       
            bool returnContours;
            bool return3D;
            
            std::vector<EoiFilter*> filters;     
            ClusterizationMethod* clusterization_method;
            
            /// <summary>
            /// Functon detects objects in image
            /// </summary>
            /// <param name="image">Destination image</param>
            /// <returns>Vector of rects of found objects</returns>  
            std::vector<ExtendedObjectInfo> Detect(const InfoImage& image, int seq);
            
            virtual std::vector<ExtendedObjectInfo> Detect2(const InfoImage& image, int seq) = 0;
            
            /// <summary>
            /// Functon checks objects params on image
            /// </summary>
            /// <param name="image">Destination image</param>
            /// <param name="rects">List of rects to be checked</param>
            /// <returns>Vector of rects of appropriate objects</returns>  	
            void Check(const InfoImage& image, std::vector<ExtendedObjectInfo>* rects);
            
            /// <summary>
            /// Functon checks rect params on image
            /// </summary>
            /// <param name="image">Destination image</param>
            /// <param name="rect">Rect to be checked</param>
            /// <returns>True if object is presented, false overwice</returns>  	
            virtual bool Check2(const InfoImage& image, ExtendedObjectInfo& rect) = 0;
            
            void Extract(const InfoImage& image, std::vector<ExtendedObjectInfo>* rects);
            
            virtual void Extract2(const InfoImage& image, ExtendedObjectInfo& rect) = 0;
    
            ObjectBase* parent_base;
            
            void setProbability(double prob);
            
            void set_extracted_info(ExtendedObjectInfo &eoi, std::string key, std::string value);
            void set_extracted_info(ExtendedObjectInfo &eoi, std::string key, int value);
            void set_extracted_info(ExtendedObjectInfo &eoi, std::string key, double value);
            
            
                                
        protected:    
            int prev_seq;
            std::vector<ExtendedObjectInfo> saved_answer;
            
            bool inited;            
            double Probability;            
                        
        private:
            

    };
}

#endif // _ATTRIBUTE_
