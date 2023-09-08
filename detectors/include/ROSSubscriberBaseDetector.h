#ifndef _ROS_SUBSCRIBER_BASE_DETECTOR_
#define _ROS_SUBSCRIBER_BASE_DETECTOR_

#include "ObjectIdentifier.h"
#include <ros/ros.h>
#include <message_filters/subscriber.h>
#include <message_filters/cache.h>

namespace eod{

    
    class ROSSubscriberSuperBaseAttribute : public Attribute{
    public:
        ROSSubscriberSuperBaseAttribute(){}    
        
        virtual std::vector<ExtendedObjectInfo> Detect2(const InfoImage& image, int seq) = 0;       
        virtual bool Check2(const InfoImage& image, ExtendedObjectInfo& rect) = 0;       
        virtual void Extract2(const InfoImage& image, ExtendedObjectInfo& rect) = 0;
        
        virtual void Connect2ROS(ros::NodeHandle nh) = 0;
        
    };
    
    template<class T>
    class ROSSubscriberBaseAttribute : public ROSSubscriberSuperBaseAttribute{
       
    public:       
        ROSSubscriberBaseAttribute(std::string topic_name, float timelag);
              
        virtual std::vector<ExtendedObjectInfo> Detect2(const InfoImage& image, int seq) = 0;       
        virtual bool Check2(const InfoImage& image, ExtendedObjectInfo& rect) = 0;       
        virtual void Extract2(const InfoImage& image, ExtendedObjectInfo& rect) = 0;
                          
        const std::string topic_name() const{return topic_name_;}
       
        void Connect2ROS(ros::NodeHandle nh);
        
        void callback(const boost::shared_ptr<T>&);        
       
    protected:    
        std::string topic_name_;
        float timelag_;
       
        message_filters::Subscriber<T>* subscriber_;
        message_filters::Cache<T>* cache_;                     
    };

    template<class T>
    ROSSubscriberBaseAttribute<T>::ROSSubscriberBaseAttribute(std::string topic_name, float timelag){
        Type = ROS_SUB_BASE_A;
        topic_name_ = topic_name;
        timelag_ = timelag;
    }
    
    template<class T>
    void ROSSubscriberBaseAttribute<T>::Connect2ROS(ros::NodeHandle nh){
        subscriber_ = new message_filters::Subscriber<T>(nh, topic_name_, 1);         
        cache_ = new message_filters::Cache<T>(*subscriber_, 100);             
        cache_->registerCallback(&ROSSubscriberBaseAttribute::callback, this);
    }
    
    template<class T>
    void ROSSubscriberBaseAttribute<T>::callback(const boost::shared_ptr<T>& msg){
        //printf("Got message\n");        
    }
   
}


#endif //_ROS_SUBSCRIBER_BASE_DETECTOR_
