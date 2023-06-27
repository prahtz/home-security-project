CC = gcc -std=c++17

headers = ./headers/

exceptions = ./exceptions/

output = hsp

ifeq ($(MAKECMDGOALS), linux)
sources = $(filter-out ./source/RCSwitch.cpp, $(wildcard ./source/*.cpp))
libs = -lstdc++ -lstdc++fs -lpthread -lcrypt -lcurlpp -lcurl -lssl -lcrypto -ljwt -lm
else ifeq ($(MAKECMDGOALS), linux_debug)
debug = -g
sources = $(filter-out ./source/RCSwitch.cpp, $(wildcard ./source/*.cpp))
libs = -lstdc++ -lstdc++fs -lpthread -lcrypt -lcurlpp -lcurl -lssl -lcrypto -ljwt -lm
else ifeq ($(MAKECMDGOALS), test)
sources = $(filter-out ./source/RCSim.cpp, $(wildcard ./source/*.cpp))
libs = -lstdc++ -lstdc++fs -lpthread -lcrypt -lcurlpp -lcurl -lwiringPi -lwiringPiDev -lssl -lcrypto -ljwt -lm
CXXFLAGS = -DRPI
else
sources = $(filter-out ./source/RCSim.cpp, $(wildcard ./source/*.cpp))
libs = -lstdc++ -lstdc++fs -lpthread -lcrypt -lcurlpp -lcurl -lwiringPi -lwiringPiDev -lssl -lcrypto -ljwt -lm
CXXFLAGS = -DRPI
endif

raspbian: $(sources)
	$(CC) $(CXXFLAGS) $(sources) -I$(headers) -I$(exceptions) -o $(output) $(libs)

linux: $(sources)
	$(CC) $(sources) -I$(headers) -I$(exceptions) -o $(output) $(libs)

linux_debug: $(sources)
	$(CC) $(debug) $(sources) -I$(headers) -I$(exceptions) -o $(output) $(libs)

test: $(sources)
	$(CC) $(CXXFLAGS) $(sources) -I$(headers) -I$(exceptions) -o test $(libs)



