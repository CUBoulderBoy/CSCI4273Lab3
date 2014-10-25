#include "ThreadPool.h"

#include <stdio.h>
#include <stdlib.h>

using namespace std;


void* startThread(void* arg);

void* startThread(void* arg)
{
    cout << "starting thread" << endl;
    ThreadPool* tp = (ThreadPool*) arg;
    // threadIndex threadStruct = *ti;
    // ThreadPool* tp = threadStruct.tp;
    // int i = threadStruct.index;

    tp->execute_thread();
    return NULL;
}

ThreadPool::ThreadPool(size_t threadCount)
{
    m_threadCount = threadCount;
    int err;

    m_threads = new pthread_t[threadCount];
    // m_sems = new sem_t[threadCount];
    // m_available = new bool[threadCount];
    // m_fn_ptr = new function_pointer[threadCount];
    // m_arg = new void*[threadCount];

    for (int i = 0; i < m_threadCount; i++) {
        sem_t mutex;
        err = sem_init(&mutex, 0, 1);
        // if (err < 0) {
        //     cout << "error: " << strerror(errno) << endl;
        //     exit(1);
        // }

        sem_wait(&mutex);

        pthread_t tid;
        err = pthread_create(&tid, NULL, startThread, (void*) this);
        if (err != 0) {
            cout << "pthread_create() failed: " << err << endl;
            exit(1);
        }
        m_sems[tid] = mutex;
        m_available[tid] = true;
        m_threads[i] = tid;
    }
    cout << m_threadCount << " threads created by the thread pool" << endl;
}

ThreadPool::~ThreadPool()
{
    // delete[] m_available;
    // delete[] m_sems;
}

int ThreadPool::dispatch_thread(void dispatch_function(void*), void *arg)
{

    for (int i = 0; i < m_threadCount; i++) {
        pthread_t tid = m_threads[i];
        cout << "dispatching " << tid << endl;

        if (m_available[tid]) {
            m_fn_ptr[tid] = dispatch_function;
            m_arg[tid] = arg;
            // m_thread_index = i;
            m_available[tid] = false;
            sem_post(&(m_sems[tid]));
            return i;
        }
    }
    return -1;
}

bool ThreadPool::thread_avail()
{
    for (int i = 0; i < m_threadCount; i++) {
        if (m_available[m_threads[i]]) {
            return true;
        }
    }
    return false;
}

void ThreadPool::execute_thread()
{
    pthread_t tid = pthread_self();
    while (true) {
        cout << tid << " waiting\n";
        sem_wait(&m_sems[tid]);
        cout << tid << " going\n";
        cout << m_fn_ptr[tid] << endl;
        (*(m_fn_ptr[tid]))(m_arg[tid]);
        m_available[tid] = true;
    }
}