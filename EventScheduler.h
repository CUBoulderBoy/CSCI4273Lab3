#include <iostream>
#include <chrono>
#include <thread>
#include <future>

// int main()
// {
//     // Use async to launch a function (lambda) in parallel
//     std::async(std::launch::async, [] () {
//         // Use sleep_for to wait specified time (or sleep_until).
//         std::this_thread::sleep_for( std::chrono::seconds{1});
//         // Do whatever you want.
//         std::cout << "Lights out!" << std::endl;
//     } );
//     std::this_thread::sleep_for( std::chrono::seconds{2});
//     std::cout << "Finished" << std::endl;
// }

class EventScheduler
{
public:
    EventScheduler(size t maxEvents);
    ∼EventScheduler();
    int eventSchedule(void evFunction(void *), void *arg, int timeout);
    void eventCancel(int eventId);
};


int EventScheduler::eventSchedule(void evFunction(void *), void *arg, int timeout)
{

}