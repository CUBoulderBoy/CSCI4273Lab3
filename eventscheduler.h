#ifndef _EVENT_SCHEDULER_
#define _EVENT_SCHEDULER_

#include <vector>
#include <queue>
#include <mutex>
#include "ThreadPool.h"

typedef void (*function_pointer)(void*);

struct Event
{
    function_pointer fn_ptr;
    void* arg;
    int trigger_time;
    int id;
};

class CompareEvent {
public:
    bool operator()(Event& e1, Event& e2) { return e2.trigger_time < e1.trigger_time; };
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
    struct timeval m_tv;

    static void coordinateEvent(void* arg);
};

#endif
