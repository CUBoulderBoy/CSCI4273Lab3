#include "EventScheduler.h"
#include <pthread.h>
#include <string.h>
#include <iostream>
#include <sys/select.h>
#include <sys/time.h>

#define LOGGING 1

using namespace std;

void EventScheduler::coordinateEvent(void* arg)
{
    EventScheduler* es = (EventScheduler*) arg;
    timeval tim;
    Event e;

    while (true) {
        gettimeofday(&tim, NULL);

        es->m_mutex.lock();
        if (!es->m_queue.empty()) {
            e = es->m_queue.top();
            es->m_mutex.unlock();

            // wait for first timeout
            es->m_tv = {e.trigger_time - tim.tv_sec, 0};
            if (select(0, NULL, NULL, NULL, &(es->m_tv)) < 0) {
                cout << "error with select: " << strerror(errno) << endl;
                exit(1);
            }

            // make sure we have the same top element
            es->m_mutex.lock();
            e = es->m_queue.top();
            es->m_queue.pop();

            // check if cancelled
            for (std::vector<int>::iterator i = es->m_cancelled.begin(); i != es->m_cancelled.end(); ++i) {
                if (e.id == *i) {
                    es->m_cancelled.erase(i);
                    es->m_mutex.unlock();
                    return;
                }
            }
            es->m_mutex.unlock();

            // call event function
            (*(e.fn_ptr))(e.arg);
        }
        else {
            es->m_mutex.unlock();
        }
    }

    // es->m_mutex.lock();
    // Event e = es->m_queue.top();
    // es->m_queue.pop();
    // es->m_mutex.unlock();

    // if (LOGGING) cout << "scheduling event " << e.id << endl;
    // struct timeval tv = {0, e.trigger_time};
    // if (select(0, NULL, NULL, NULL, &tv) < 0) {
    //     cout << "error with select: " << strerror(errno) << endl;
    //     exit(1);
    // }

    // es->m_mutex.lock();
    // for (std::vector<int>::iterator i = es->m_cancelled.begin(); i != es->m_cancelled.end(); ++i) {
    //     if (e.id == *i) {
    //         es->m_mutex.unlock();
    //         es->m_cancelled.erase(i);
    //         return;
    //     }
    // }
    // es->m_mutex.unlock();
    // (*(e.fn_ptr))(e.arg);
}

EventScheduler::EventScheduler(size_t maxEvents)
{
    m_current_id = 0;
    m_max_events = maxEvents;
    m_pool = new ThreadPool(maxEvents + 1);
    m_pool->dispatch_thread(coordinateEvent, this);
}

EventScheduler::~EventScheduler()
{
    delete m_pool;
}

int EventScheduler::eventSchedule(void evFunction(void *), void *arg, int timeout)
{
    timeval tim;
    gettimeofday(&tim, NULL);
    int event_sec = timeout + tim.tv_sec;
    Event e = {evFunction, arg, event_sec, m_current_id};

    m_mutex.lock();
    if (event_sec < m_queue.top().trigger_time) {
        m_tv = {event_sec, 0};
    }
    m_queue.push(e);
    m_mutex.unlock();
    return m_current_id++;
}

void EventScheduler::eventCancel(int eventId)
{
    m_mutex.lock();
    m_cancelled.push_back(eventId);
    if (LOGGING) cout << "cancelling event " << eventId << endl;
    m_mutex.unlock();
}
