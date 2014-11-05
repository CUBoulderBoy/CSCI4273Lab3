#include "threadpool.h"

#include <stdio.h>
#include <stdlib.h>

#define LOGGING 0

using namespace std;

void* ThreadPool::startThread(void* arg)
{
    if (LOGGING) cout << "starting thread " << pthread_self() << endl;
    ThreadPool* tp = (ThreadPool*) arg;
    tp->execute_thread();
    return NULL;
}

ThreadPool::ThreadPool(size_t threadCount)
{
    m_threadCount = threadCount;
    m_threads = new pthread_t[threadCount];
    int err;

    // create every thread with its own semaphore and available boolean
    for (int i = 0; i < m_threadCount; i++) {
        sem_t mutex;
        err = sem_init(&mutex, 0, 1);
        if (err < 0) {
            cout << "error: " << strerror(errno) << endl;
            exit(1);
        }

        // call sem wait so the threads will block when they start running
        sem_wait(&mutex);

        pthread_t tid;
        err = pthread_create(&tid, NULL, ThreadPool::startThread, (void*) this);
        if (err != 0) {
            cout << "pthread_create() failed: " << err << endl;
            exit(1);
        }
        m_sems[tid] = mutex;
        m_available[tid] = true;
        m_threads[i] = tid;
    }
    if (LOGGING) cout << m_threadCount << " threads created by the thread pool" << endl;
}

ThreadPool::~ThreadPool()
{
    delete[] m_threads;
}

int ThreadPool::dispatch_thread(void dispatch_function(void*), void *arg)
{
    // look for an available thread
    for (int i = 0; i < m_threadCount; i++) {
        pthread_t tid = m_threads[i];
        if (LOGGING) cout << "dispatching " << tid << endl;
        if (m_available[tid]) {
            // each tid has its own function pointer
            m_fn_ptr[tid] = dispatch_function;
            m_arg[tid] = arg;

            // set the thread to be unavailable and call sem_post to unblock it
            m_available[tid] = false;
            sem_post(&(m_sems[tid]));
            return 0;
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
        if (LOGGING) cout << tid << " waiting\n";
        sem_wait(&m_sems[tid]); // thread will be blocked here until dispatch thread is called
        if (LOGGING) cout << tid << " going\n";
        (*(m_fn_ptr[tid]))(m_arg[tid]);
        m_available[tid] = true;
    }
}