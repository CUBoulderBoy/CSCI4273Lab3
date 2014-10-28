#ifndef _THREAD_POOL_
#define _THREAD_POOL_

#include <map>
#include <pthread.h>
#include <semaphore.h>
#include <string.h>
#include <mutex>
#include <iostream>

typedef void (*function_pointer)(void*);

class ThreadPool
{
public:
    ThreadPool(size_t threadCount);
    ~ThreadPool();
    int dispatch_thread(void dispatch_function(void*), void *arg);
    bool thread_avail();

private:
    size_t m_threadCount;
    pthread_t* m_threads;;
    std::map<pthread_t, sem_t> m_sems;
    std::map<pthread_t, bool> m_available;
    std::map<pthread_t, function_pointer> m_fn_ptr;
    std::map<pthread_t, void*> m_arg;

    void execute_thread();
    static void* startThread(void* arg);
};

#endif
