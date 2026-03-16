CXX = g++
CXXFLAGS = -std=c++14 -O2 -Wall -I /usr/local/include/boost/

TARGET = ../bin/hw2
SRC = main.cpp partition.cpp
HDR = graph.h partition.h

all: $(TARGET)

$(TARGET): $(SRC) $(HDR)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRC)

clean:
	rm -f $(TARGET) *.o test.txt
