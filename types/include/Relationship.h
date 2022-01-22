#ifndef RELATION_SHIP_H
#define RELATION_SHIP_H

//#include "SimpleObject.h"
#include "ExtendedObjectInfo.h"
#include "tinyxml.h"

namespace eod{

    enum RelationLinearity{
        BILINEAR_R,
        MULTILINEAR_R,
    };
        
    enum RelationTypes{
        UNK_R = 0,
        IM_RANGE_R,
        LOG_AND_R,
        LOG_NOT_R,
        LOG_OR_R,
        TD_RANGE_R,
        SPACE_IN_R,
        SPACE_OUT_R,
        SPACE_UP_R,
        SPACE_DOWN_R,
        SPACE_LEFT_R,
        SPACE_RIGHT_R,
        SIZE_SAME_R,
        SIZE_BIGGER_R,
        SIZE_SMALLER_R,
        SIZE_PERCENT_R,        
        SAME_EXTR_INFO_R,
        SAME_DIST_IMAGE_R
    };
    
    RelationTypes getRelationTypeFromName(std::string name);
    
    class RelationShip{
    public:
      std::string Name;              
      int ID;
      RelationLinearity linearity;

      /// <summary>
      /// Default constructor
      /// </summary>		           
      RelationShip();
      
      /// <summary>
      /// Prints information about relation.
      /// </summary>		   
      //virtual void printInfo() = 0;
      
      /// <summary>
      /// Checks if objects are satisfied to relation
      /// </summary>		 
      /// <param name="A">Pointer to first object</param>
      /// <param name="B">Pointer to second object</param>
      /// <returns>True or false</returns>	      
      virtual bool checkRelation(const cv::Mat& image, ExtendedObjectInfo* A, ExtendedObjectInfo* B) = 0;
      
      //virtual bool readFromXML(TiXmlElement* relation_tag) = 0;
      
      bool setName(const char*);
	
    private:
        
    protected:
        int Type;        
        bool inited;

		
    };

    class MultiLinearRelationShip : public RelationShip{
    public:
        MultiLinearRelationShip();

        virtual MultiLinearRelationShip * clone() const = 0;

        int graph_v_color;

        bool checkRelation(const cv::Mat& image, ExtendedObjectInfo* A, ExtendedObjectInfo* B);

        virtual std::vector<int> checkMultilinearRelation(const cv::Mat& image, int seq, const ExtendedObjectInfo& A, const ExtendedObjectInfo& B) = 0;

        bool get_vid_from_index(int index, int& vid);
        void set_vid_for_index(int index, int vid);

    protected:
        int prev_seq;
        std::map<int, int> Index2VID;
    };
}

#endif //RELATION_SHIP_H
