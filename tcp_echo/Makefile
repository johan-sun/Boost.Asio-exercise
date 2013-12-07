CXXFLAGS = -std=c++11 -Wall -O2
LIBS ?= -lboost_system
OS ?= $(shell uname -s)
ARCH ?= $(shell uname -m)
SOURCES = $(wildcard *.cpp)
TARGEST = $(patsubst %.cpp, %.run, $(SOURCES))
ifeq ($(OS), Linux)
	LIBS += -lboost_thread
endif

ifeq ($(OS), Darwin)
	LIBS += -lboost_thread-mt
endif

ifeq ($(ARCH), x86_64)
	CXXFLAGS += -m64
else
	CXXFLAGS += -m32
endif



.PHONY:clean all

all:$(TARGEST)

%.run:%.o
	$(CXX) -o $@ $< $(LIBS)
#tcp_sync_client.run:tcp_sync_client.o
#	$(CXX) -o $@ $< $(LIBS) 
#
#tcp_sync_server.run:tcp_sync_server.o
#	$(CXX) -o $@ $< $(LIBS)


clean:
	rm -rf *.o $(TARGEST)
