#include "EventScheduler.h"
#include <pthread.h>
#include <string.h>
#include <iostream>
#include <sys/select.h>

using namespace std;

void EventScheduler::coordinateEvent(void* arg)
{
    EventScheduler* es = (EventScheduler*) arg;

    Event e = es->m_queue.top();

    struct timeval tv = {e.trigger_time, 0};
    if (select(0, NULL, NULL, NULL, &tv) < 0) {
        cout << "error with select: " << strerror(errno) << endl;
        exit(1);
    }

    es->m_mutex.lock();
    for (std::vector<int>::iterator i = es->m_cancelled.begin(); i != es->m_cancelled.end(); ++i) {
        if (e.id == *i) {
            es->m_mutex.unlock();
            return;
        }
    }
    es->m_queue.pop();
    es->m_mutex.unlock();
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
    Event e = {evFunction, arg, timeout, m_current_id};
    m_queue.push(e);

    m_pool->dispatch_thread(coordinateEvent, this);

    return m_current_id++;
}

void EventScheduler::eventCancel(int eventId)
{
    m_mutex.lock();
    m_cancelled.push_back(eventId);
    m_mutex.unlock();
}
