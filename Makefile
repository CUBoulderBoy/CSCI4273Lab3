all:
	# g++ EventScheduler.cpp
	g++ main.cpp ThreadPool.cpp EventScheduler.cpp -lpthread -std=c++11