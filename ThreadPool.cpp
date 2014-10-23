#include "ThreadPool.h"

using namespace std;

void* startThread(void* arg);

void* startThread(void* arg)
{
    cout << "starting thread" << endl;
    ThreadPool* tp = (ThreadPool*) arg;
    tp->execute_thread();
    return NULL;
}

ThreadPool::ThreadPool(size_t threadCount)
{
    m_threadCount = threadCount;
    int err;

    m_sems = new sem_t[threadCount];
    m_available = new bool[threadCount];

    for (int i = 0; i < m_threadCount; i++) {
        sem_t mutex;
        sem_init(&mutex, 0, 1);
        m_sems[i] = mutex;

        // sem_wait(mutex);
        int value;
      sem_getvalue(&mutex, &value);
      printf("The value of the semaphors is %d\n", value);
        // sem_wait(mutex);
      sem_post(&mutex);

              sem_getvalue(&mutex, &value);
      printf("The value of the semaphors is %d\n", value);

      exit(0);
        m_available[i] = true;
        // m_sems[i].lock();

        pthread_t tid;
        err = pthread_create(&tid, NULL, startThread, (void*) this);
        if (err != 0) {
            cout << "pthread_create() failed: " << err << endl;
            exit(1);
        }
        // m_threads.push_back(tid);
    }
    cout << m_threadCount << " threads created by the thread pool" << endl;

    // for (int i = 0; i < m_threadCount; i++) {
    //     tid.join();
    // }
}

ThreadPool::~ThreadPool()
{
    delete[] m_available;
    delete[] m_sems;
}

int ThreadPool::dispatch_thread(void dispatch_function(void*), void *arg)
{
    cout << "dispatch called" << endl;
    // (*dispatch_function)(arg);
    // cout << "dispatch done" << endl;
    for (int i = 0; i < m_threadCount; i++) {
        if (m_available[i]) {
            // m_sems[i].lock();
            m_fn_ptr = dispatch_function;
            m_arg = arg;
            m_thread_index = i;
            m_available[m_thread_index] = false;
            // sem_post(&m_sems[i]);
            // m_sems[i].unlock();
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

void ThreadPool::execute_thread()
{
    int count = 0;
    cout << "execute_thread" << endl;
    // m_sems[m_thread_index].lock();
    while (true) {
        sem_wait(&m_sems[m_thread_index]);
        cout << "here" << endl;
        (*m_fn_ptr)(m_arg);
        cout << "there" << endl;
        m_available[m_thread_index] = true;
        // m_sems[m_thread_index].lock();
        count++;
        cout << count << endl;
    }
}