#ifndef _EVENT_SCHEDULER_
#define _EVENT_SCHEDULER_

#include <vector>
#include <queue>
#include <mutex>
#include "threadpool.h"

typedef void (*function_pointer)(void*);

struct Event
{
    function_pointer fn_ptr;
    void* arg;
    timeval trigger_time;
    int id;
};

class CompareEvent {
public:
    bool operator()(Event& e1, Event& e2) { 
        if (e2.trigger_time.tv_sec < e1.trigger_time.tv_sec) return true;
        if (e2.trigger_time.tv_sec == e1.trigger_time.tv_sec && e2.trigger_time.tv_usec < e1.trigger_time.tv_usec) return true;
        return false;
    };
};

class EventScheduler
{
public:
    EventScheduler(size_t maxEvents);
    ~EventScheduler();
    int eventSchedule(void evFunction(void *), void *arg, int timeout);
    void eventCancel(int eventId);

private:
    std::priority_queue<Event, std::vector<Event>, CompareEvent> m_queue;
    size_t m_max_events;
    ThreadPool* m_pool;
    int m_current_id;
    std::mutex m_mutex;
    std::vector<int> m_cancelled;
    std::vector<int> m_scheduled;
    timeval m_tv;

    static void coordinateEvent(void* arg);
};

#endif
