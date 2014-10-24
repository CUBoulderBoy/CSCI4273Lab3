#include "ThreadPool.h"

#include <stdio.h>
#include <stdlib.h>

using namespace std;

struct threadIndex
{
    int index;
    ThreadPool* tp;
};

void* startThread(void* arg);

void* startThread(void* arg)
{
    cout << "starting thread" << endl;
    threadIndex* ti = (threadIndex*) arg;
    threadIndex threadStruct = *ti;
    ThreadPool* tp = threadStruct.tp;
    int i = threadStruct.index;

    // sem_t* m_sems = tp->m_sems;
    // bool* m_available = tp->m_available;
    // function_pointer* m_fn_ptr = tp->m_fn_ptr;
    // void** m_arg = tp->m_arg;

    // int count = 0;
    // while (true) {
    //     sem_wait(&m_sems[i]);
    //     cout << "here" << endl;
    //     (*m_fn_ptr[i])(m_arg[i]);
    //     cout << "there" << endl;
    //     m_available[i] = true;
    //     count++;
    //     cout << count << endl;
    // }
    tp->execute_thread(i);
    return NULL;
}

ThreadPool::ThreadPool(size_t threadCount)
{
    m_threadCount = threadCount;
    int err;

    m_sems = new sem_t[threadCount];
    m_available = new bool[threadCount];
    m_fn_ptr = new function_pointer[threadCount];
    m_arg = new void*[threadCount];

    for (int i = 0; i < m_threadCount; i++) {
        sem_t mutex;
        err = sem_init(&mutex, 0, 1);
        if (err < 0) {
            cout << "error: " << strerror(errno) << endl;
            exit(1);
        }
        m_sems[i] = mutex;

        sem_wait(&mutex);
        m_available[i] = true;

        pthread_t tid;
        threadIndex ti = {i, this};
        threadIndex* ti_ptr = &ti;
        err = pthread_create(&tid, NULL, startThread, (void*) ti_ptr);
        if (err != 0) {
            cout << "pthread_create() failed: " << err << endl;
            exit(1);
        }
    }
    cout << m_threadCount << " threads created by the thread pool" << endl;
}

ThreadPool::~ThreadPool()
{
    delete[] m_available;
    delete[] m_sems;
}

int ThreadPool::dispatch_thread(void dispatch_function(void*), void *arg)
{
    cout << "dispatch called" << endl;

    for (int i = 0; i < m_threadCount; i++) {
        if (m_available[i]) {
            m_fn_ptr[i] = dispatch_function;
            m_arg[i] = arg;
            m_thread_index = i;
            m_available[m_thread_index] = false;
            sem_post(&m_sems[i]);
            return i;
        }
    }
    return -1;
}

bool ThreadPool::thread_avail()
{
    for (int i = 0; i < m_threadCount; i++) {
        if (m_available[i]) {
            return true;
        }
    }
    return false;
}

void ThreadPool::execute_thread(int i)
{
    int count = 0;
    cout << "execute_thread" << endl;
    while (true) {
        sem_wait(&m_sems[i]);
        cout << "here" << endl;
        (*m_fn_ptr[i])(m_arg[i]);
        cout << "there" << endl;
        m_available[i] = true;
        count++;
        cout << count << endl;
    }
}