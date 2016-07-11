//
//  AirQueue.cpp
//  kxmovie
//
//  Created by penghao on 16/1/25.
//
//

#include "AirQueue.hpp"
#include <pthread/pthread.h>
#include<unistd.h>
#include <signal.h>

namespace AirCpp {
    
    void Queue::pushOperation(Operation *operation){
        m_listOperations.push_back(operation);
    }
    
}