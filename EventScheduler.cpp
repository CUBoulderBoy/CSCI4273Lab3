#include "eventscheduler.h"
#include <pthread.h>
#include <string.h>
#include <iostream>
#include <sys/select.h>
#include <sys/time.h>

#define LOGGING 0

using namespace std;

bool compareTime(timeval t1, timeval t2) {
    if (t2.tv_sec < t1.tv_sec) return true;
    if (t2.tv_sec == t1.tv_sec && t2.tv_usec < t1.tv_usec) return true;
    return false;
}

void EventScheduler::coordinateEvent(void* arg)
{
    EventScheduler* es = (EventScheduler*) arg;
    timeval tim;
    Event e;
    bool cancelled;

    if (LOGGING) cout << "coordinateEvent\n";

    while (true) {
        gettimeofday(&tim, NULL);

        es->m_mutex.lock();
        if (!es->m_queue.empty()) {
            e = es->m_queue.top();

            // wait for first timeout
            int msec = e.trigger_time.tv_usec - tim.tv_usec;
            int sec = e.trigger_time.tv_sec - tim.tv_sec;
            if (msec < 0) {
                msec += 1000000;
                sec--;
            }
            es->m_tv = {sec, msec};
            es->m_mutex.unlock();

            if (LOGGING) cout << "setting timer for " << sec << " " << msec << endl;
            if (select(0, NULL, NULL, NULL, &(es->m_tv)) < 0) {
                cout << "error with select: " << strerror(errno) << endl;
                exit(1);
            }

            // make sure we have the same top element
            es->m_mutex.lock();
            e = es->m_queue.top();
            es->m_queue.pop();

            // check if cancelled
            cancelled = false;
            for (std::vector<int>::iterator i = es->m_cancelled.begin(); i != es->m_cancelled.end(); ++i) {
                if (e.id == *i) {
                    es->m_cancelled.erase(i);
                    if (LOGGING) cout << "cancelled event " << e.id << " would have run now\n";
                    cancelled = true;
                    break;
                }
            }
            es->m_mutex.unlock();

            if (!cancelled) {
                // call event function
                (*(e.fn_ptr))(e.arg);
            }

        }
        else {
            es->m_mutex.unlock();
        }
    }
}

EventScheduler::EventScheduler(size_t maxEvents)
{
    m_current_id = 0;
    m_max_events = maxEvents;

    // add one extra thread to the pool for the main coordinator
    m_pool = new ThreadPool(maxEvents + 1);
    m_pool->dispatch_thread(coordinateEvent, this);
}

EventScheduler::~EventScheduler()
{
    delete m_pool;
}

int EventScheduler::eventSchedule(void evFunction(void *), void *arg, int timeout)
{
    timeval tim, event_time;
    gettimeofday(&tim, NULL);
    int secs = 0;
    int msecs = timeout * 1000 + tim.tv_usec;
    while (msecs >= 1000000) {
        msecs -= 1000000;
        secs++;
    }
    event_time = {tim.tv_sec + secs, msecs};
    Event e = {evFunction, arg, event_time, m_current_id};

    m_mutex.lock();
    if (LOGGING) cout << "scheduling event " << m_current_id << " at " << event_time.tv_sec << " " << event_time.tv_usec << endl;
    if (!m_queue.empty() && compareTime(event_time, m_queue.top().trigger_time))
    {
        if (LOGGING) cout << "reseting first event time" << endl;
        m_tv = {event_time.tv_sec, event_time.tv_usec};
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
