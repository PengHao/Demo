//
//  AirThreadLock.hpp
//  kxmovie
//
//  Created by penghao on 16/1/25.
//
//

#ifndef AirThreadLock_hpp
#define AirThreadLock_hpp

#include <stdio.h>
#include <sys/sem.h>
#include <sys/ipc.h>
#include <sys/semaphore.h>
#include <sys/types.h>
#include <pthread.h>



namespace AirCpp {
    class Lock{
    protected:
        
        pthread_mutex_t mutex;
    public:
        int init();
        
        int lock();
        
        int unlock();
        
        int try_lock();
        
        int try_destroy();
    };
    
    
    class Semaphore{
    protected:
        sem_t thread_sem;
    public:
        int init(bool shared);
        
        int wait();
        
        int post();
        
        int destroy();
        
        ~Semaphore();
    };
    
    
    class RWLock{
    protected:
        pthread_rwlock_t rwlock;
    public:
        int init();
        
        int rdlock();
        
        int wrlock();
        
        int try_rdlock();
        
        int try_wrlock();
        
        int unlock();
        
        int destroy();
    };
}
#endif /* AirThreadLock_hpp */
