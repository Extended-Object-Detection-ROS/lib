#if (USE_ROS)
#include "ROSSubscriberBaseDetector.h"

namespace eod{
    
    ROSSubscriberBaseAttribute::ROSSubscriberBaseAttribute(std::string topic_name){
        Type = ROS_SUB_BASE_A;
        topic_name_ = topic_name;
    }
    
    template<class M>
    void ROSSubscriberBaseAttribute::callback(const M& message){        
        
    }
    
}
#endif
