CC = g++ -std=c++17

headers = ./headers/

exceptions = ./exceptions/

output = hsp

ifeq ($(MAKECMDGOALS), linux)
sources = $(filter-out ./source/RCSwitch.cpp, $(wildcard ./source/*.cpp))
libs = -lpthread -lcrypt -lcurlpp -lcurl
else
sources = $(filter-out ./source/RCSim.cpp, $(wildcard ./source/*.cpp))
libs = -lpthread -lcrypt -lcurlpp -lcurl -lwiringPi -lwiringPiDev
CXXFLAGS = -DRPI
endif

raspbian: $(sources)
	$(CC) $(CXXFLAGS) $(sources) -I$(headers) -I$(exceptions) -o $(output) $(libs)

linux: $(sources)
	$(CC) $(sources) -I$(headers) -I$(exceptions) -o $(output) $(libs)



