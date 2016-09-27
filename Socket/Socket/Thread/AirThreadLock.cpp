//
//  AirThreadLock.cpp
//  kxmovie
//
//  Created by penghao on 16/1/25.
//
//

#include "AirThreadLock.hpp"


namespace AirCpp {
    
    int Lock::init(){
        return pthread_mutex_init(&mutex, NULL);
    }
    
    int Lock::lock(){
        return pthread_mutex_lock(&mutex);
    }
    
    int Lock::unlock(){
        return pthread_mutex_unlock(&mutex);
    }
    
    int Lock::try_lock(){
        return pthread_mutex_trylock(&mutex);
    }
    
    int Lock::try_destroy(){
        return pthread_mutex_destroy(&mutex);
    }
    
    
    int Semaphore::init(bool shared){
        this->destroy();
        return sem_init(&thread_sem, shared, 0);
    }
    
    int Semaphore::wait(){
        return sem_wait(&thread_sem);
    }
    
    int Semaphore::post(){
        return sem_post(&thread_sem);
    }
    
    int Semaphore::destroy(){
        return sem_destroy(&thread_sem);
    }
    
    Semaphore::~Semaphore(){
        this->destroy();
    }
    
    int RWLock::init(){
        return pthread_rwlock_init(&rwlock, NULL);
    }
    
    int RWLock::rdlock(){
        return pthread_rwlock_rdlock(&rwlock);
    }
    
    int RWLock::wrlock(){
        return pthread_rwlock_wrlock(&rwlock);
    }
    
    int RWLock::try_rdlock(){
        return pthread_rwlock_tryrdlock(&rwlock);
    }
    
    int RWLock::try_wrlock(){
        return pthread_rwlock_trywrlock(&rwlock);
    }
    
    int RWLock::unlock(){
        return pthread_rwlock_unlock(&rwlock);
    }
    
    int RWLock::destroy(){
        return pthread_rwlock_destroy(&rwlock);
    }
}