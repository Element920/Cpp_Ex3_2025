CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -g
SFML_FLAGS = -lsfml-graphics -lsfml-window -lsfml-system

# Source files
SOURCES = Player.cpp Game.cpp Baron.cpp Governor.cpp Spy.cpp General.cpp Judge.cpp Merchant.cpp
OBJECTS = $(SOURCES:.cpp=.o)

# Targets
.PHONY: all clean test valgrind gui help

all: Main

# Original main target
Main: Demo.cpp $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o Main Demo.cpp $(OBJECTS)

# GUI version with SFML
gui: CoupGUI.cpp $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o coup_gui CoupGUI.cpp $(OBJECTS) $(SFML_FLAGS)

test: TestRunner.cpp $(OBJECTS)
	$(CXX) $(CXXFLAGS) -o test TestRunner.cpp $(OBJECTS)
	./test

valgrind: Main
	valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes ./Main

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o Main test coup_gui

# Dependencies
Player.o: Player.cpp Player.hpp Role.hpp Game.hpp
Game.o: Game.cpp Game.hpp Player.hpp
Baron.o: Baron.cpp Baron.hpp Player.hpp Game.hpp
Governor.o: Governor.cpp Governor.hpp Player.hpp Game.hpp
Spy.o: Spy.cpp Spy.hpp Player.hpp Game.hpp
General.o: General.cpp General.hpp Player.hpp Game.hpp
Judge.o: Judge.cpp Judge.hpp Player.hpp Game.hpp
Merchant.o: Merchant.cpp Merchant.hpp Player.hpp Game.hpp

# Help
help:
	@echo "Available targets:"
	@echo "  all      - Build original Main executable"
	@echo "  Main     - Build original demo version"
	@echo "  gui      - Build GUI version (requires SFML)"
	@echo "  test     - Build and run tests"
	@echo "  valgrind - Run Main with valgrind"
	@echo "  clean    - Remove all object files and executables"
	@echo ""
	@echo "Note: GUI version requires SFML to be installed"
	@echo "On macOS: brew install sfml"
	@echo "On Ubuntu: sudo apt-get install libsfml-dev"