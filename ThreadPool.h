
class ThreadPool
{
public:
    ThreadPool(size_t threadCount);
    ~ThreadPool();
    int dispatch thread(void dispatch function(void*), void *arg);
    bool thread avail();
}