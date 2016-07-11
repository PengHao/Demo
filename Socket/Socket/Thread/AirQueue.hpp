//
//  AirQueue.hpp
//  kxmovie
//
//  Created by penghao on 16/1/25.
//
//

#ifndef AirQueue_hpp
#define AirQueue_hpp

#include <stdio.h>
#include <functional>
#include <list>
#include "AirOperation.h"

#define FINISHED 0x001

namespace AirCpp {
    class Queue {
        friend class Thread;
        
    private:
        std::list<const Operation *> m_listOperations;
        
        void pushOperation(Operation *operation);
        
    protected:
        
        const Operation *head_operation() {
            return *m_listOperations.begin();
        }
        
        void remove_operation(const Operation *o) {
            m_listOperations.remove(o);
        }
        
    public:
        
        Queue(){
            
        };
        
        virtual ~Queue(){
        };
    };
}

#endif /* AirQueue_hpp */
