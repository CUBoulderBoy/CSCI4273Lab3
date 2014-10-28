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
    bool cancelled;

    if (LOGGING) cout << "coordinateEvent\n";

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
    timeval tim;
    gettimeofday(&tim, NULL);
    int event_sec = timeout + tim.tv_sec;
    Event e = {evFunction, arg, event_sec, m_current_id};
    if (LOGGING) cout << "scheduling event " << m_current_id << endl;

    m_mutex.lock();
    if (!m_queue.empty() && event_sec < m_queue.top().trigger_time) {
        if (LOGGING) cout << "reseting first event time" << endl;
        m_tv = {event_sec, 0};
    }
    if (LOGGING) cout << "scheduling event for " << event_sec << endl;
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
