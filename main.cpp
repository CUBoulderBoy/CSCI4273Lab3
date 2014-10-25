#include "ThreadPool.h"

#include <iostream>
#include <unistd.h>

using namespace std;

const int MAX_TASKS = 16;

void hello(void* arg)
{
  int* x = (int*) arg;
  cout << "Hello" << *x << " from tid " << pthread_self() << endl;
}

int main(int argc, char* argv[])
{
  ThreadPool tp(4);

  for (int i = 0; i < MAX_TASKS; i++) {
    int* x = new int();
    *x = i+1;
    if (tp.thread_avail()) {
      tp.dispatch_thread(hello, (void*) x);
    }
    else {
      i--;
    }
  }

  cout << "Before sleep\n";
  unsigned int microseconds = 2000000;
  usleep(microseconds);

  cout << "Exiting app..." << endl;

  return 0;
}