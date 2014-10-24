#include <vector>
#include <pthread.h>
#include <semaphore.h>
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
    void execute_thread(int i);

private:
    size_t m_threadCount;
    int m_thread_index;
    sem_t* m_sems;
    bool* m_available;
    function_pointer* m_fn_ptr;
    void** m_arg;
};
