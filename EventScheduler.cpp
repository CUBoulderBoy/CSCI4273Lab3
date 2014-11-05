#include "eventscheduler.h"
#include <pthread.h>
#include <string.h>
#include <iostream>
#include <sys/select.h>
#include <sys/time.h>

#define LOGGING 0 // set to 1 to enable logging

using namespace std;

void EventScheduler::coordinateEvent(void* arg)
{
    EventScheduler* es = (EventScheduler*) arg;

    // pop the top event off the queue
    es->m_mutex.lock();
    Event e = es->m_queue.top();
    es->m_queue.pop();
    es->m_mutex.unlock();

    // block this thread on a select call
    if (LOGGING) cout << "scheduling event " << e.id << endl;
    if (select(0, NULL, NULL, NULL, &e.trigger_time) < 0) {
        cout << "error with select: " << strerror(errno) << endl;
        exit(1);
    }

    // make sure the event hasn't been cancelled
    es->m_mutex.lock();
    for (std::vector<int>::iterator i = es->m_cancelled.begin(); i != es->m_cancelled.end(); ++i) {
        if (e.id == *i) {
            es->m_mutex.unlock();
            es->m_cancelled.erase(i);
            return;
        }
    }
    es->m_mutex.unlock();

    // execute the event
    (*(e.fn_ptr))(e.arg);
}


EventScheduler::EventScheduler(size_t maxEvents)
{
    m_current_id = 0;
    m_max_events = maxEvents;
    m_pool = new ThreadPool(maxEvents);
}

EventScheduler::~EventScheduler()
{
    delete m_pool;
}

int EventScheduler::eventSchedule(void evFunction(void *), void *arg, int timeout)
{
    // convert the millisecond argument to seconds and a microseconds for timevals
    int secs = 0;
    int msecs = timeout * 1000;
    while (msecs >= 1000000) {
        msecs -= 1000000;
        secs++;
    }
    timeval event_time = {secs, msecs};

    // create an event and push it on the queue
    Event e = {evFunction, arg, event_time, m_current_id};
    m_mutex.lock();
    m_queue.push(e);
    m_mutex.unlock();

    // dispatch the event, it will wait until the timeout before executing
    m_pool->dispatch_thread(coordinateEvent, this);
    return m_current_id++;
}

void EventScheduler::eventCancel(int eventId)
{
    // push the event ID onto the cancelled vector
    m_mutex.lock();
    m_cancelled.push_back(eventId);
    if (LOGGING) cout << "cancelling event " << eventId << endl;
    m_mutex.unlock();
}
