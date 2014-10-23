#include <vector>
#include <pthread.h>
#include <semaphore.h>
#include <mutex>
#include <iostream>

class ThreadPool
{
public:
    ThreadPool(size_t threadCount);
    ~ThreadPool();
    int dispatch_thread(void dispatch_function(void*), void *arg);
    bool thread_avail();
    void execute_thread();

private:
    size_t m_threadCount;
    int m_thread_index;
    // pthread_t m_threads;
    // std::mutex* m_mutex_array;
    sem_t* m_sems;
    bool* m_available;
    void (*m_fn_ptr)(void*);
    void* m_arg;

};
