CXX = g++
CXXFLAGS = -std=c++17 -Wall

all: client

client: main.o Simulator.o WorldState.o
	$(CXX) $(CXXFLAGS) -o client main.o Simulator.o WorldState.o

main.o: main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp

Simulator.o: Simulator.cpp
	$(CXX) $(CXXFLAGS) -c Simulator.cpps

WorldState.o: WorldState.cpp
	$(CXX) $(CXXFLAGS) -c WorldState.cpp

clean:
	rm -f *.o client


