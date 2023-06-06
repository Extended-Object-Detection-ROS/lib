#ifndef _ROS_SUBSCRIBER_BASE_DETECTOR_
#define _ROS_SUBSCRIBER_BASE_DETECTOR_

#include "ObjectIdentifier.h"
#include <ros/ros.h>

namespace eod{

   class ROSSubscriberBaseAttribute : public Attribute{
       
   public:
       
       ROSSubscriberBaseAttribute(std::string topic_name);
       
       virtual std::vector<ExtendedObjectInfo> Detect2(const InfoImage& image, int seq) = 0;
       virtual bool Check2(const InfoImage& image, ExtendedObjectInfo& rect) = 0;
       virtual void Extract2(const InfoImage& image, ExtendedObjectInfo& rect) = 0;
       
       template<class M>
       void callback(const M& message);
       
       const std::string topic_name() const{return topic_name_;}
       
       ros::Subscriber subscriber;
       
   private:
       
       std::string topic_name_;
       
   };
   
}


#endif //_ROS_SUBSCRIBER_BASE_DETECTOR_
