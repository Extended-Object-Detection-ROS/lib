#ifndef _ARUCO_DETECTOR_
#define _ARUCO_DETECTOR_

#include <opencv2/aruco.hpp>
#include <map>
#include "ObjectIdentifier.h"


namespace eod{        
    
    class ArucoAttribute : public Attribute{
    public:	    
        /// <summary>
        /// Default constructor
        /// </summary>
        ArucoAttribute();
        
        /// <summary>
        /// Constructor with ID
        /// </summary>
        ArucoAttribute(int dictionary, double markerLen = 0);
        
        /// <summary>
        /// Detects aruco markers with 
        /// </summary>
        /// <param name="image">Destination image</param>
        /// <returns>Vector of rects of found objects</returns>
        std::vector<ExtendedObjectInfo> Detect2(const cv::Mat& image, int seq);
        
        bool Check2(const cv::Mat& image, ExtendedObjectInfo& rect);
        
        void Extract2(const cv::Mat& image, ExtendedObjectInfo& rect);
        
        void setCamParams(cv::Mat camMat, cv::Mat distCoef);        
        bool hasCamParams();
        
        
    private:
                        
        cv::aruco::PREDEFINED_DICTIONARY_NAME dict;                
        cv::Ptr<cv::aruco::Dictionary> dictionary;
                
        cv::Mat camMat;
        cv::Mat distCoef;        
        
        std::vector< std::vector<cv::Point2f> > markerCorners;
        std::vector< int > markerIds; 
        
        double markerLen;
    };

}

#endif // _ARUCO_DETECTOR_
