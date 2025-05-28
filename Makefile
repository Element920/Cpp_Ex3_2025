CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g

# Source files
SOURCES = Player.cpp Game.cpp Baron.cpp Governor.cpp Spy.cpp General.cpp Judge.cpp Merchant.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Targets
.PHONY: all clean test valgrind

all: Main

Main: Demo.cpp $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o Main Demo.cpp $(OBJECTS)

test: TestRunner.cpp $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o test TestRunner.cpp $(OBJECTS)
	./test

valgrind: Main
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./Main

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o Main test

# Dependencies
Player.o: Player.cpp Player.hpp Role.hpp Game.hpp
Game.o: Game.cpp Game.hpp Player.hpp
Baron.o: Baron.cpp Baron.hpp Player.hpp Game.hpp
Governor.o: Governor.cpp Governor.hpp Player.hpp Game.hpp
Spy.o: Spy.cpp Spy.hpp Player.hpp Game.hpp
General.o: General.cpp General.hpp Player.hpp Game.hpp
Judge.o: Judge.cpp Judge.hpp Player.hpp Game.hpp
Merchant.o: Merchant.cpp Merchant.hpp Player.hpp Game.hpp