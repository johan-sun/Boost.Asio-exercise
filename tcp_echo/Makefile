CC = gcc
CXX = g++
CXXFLAGS = -std=c++11 -Wall -O2
LIBS = -lboost_system -lboost_thread-mt
.PHONY:clean all

all:tcp_sync_client.run tcp_sync_server.run

tcp_sync_client.run:tcp_sync_client.o
	$(CXX) $(LIBS) -o $@ $<

tcp_sync_server.run:tcp_sync_server.o
	$(CXX) $(LIBS) -o $@ $<

clean:
	rm -rf *.o tcp_sync_client tcp_sync_server
