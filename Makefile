all:
	g++ -std=c++11 main.cpp network_monitor.cpp file_monitor.cpp session_monitor.cpp logger.cpp response.cpp -lpcap -o os_ids -lpthread

clean:
	rm -f os_ids
