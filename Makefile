# Makefile for Coup game
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra

# קבצי מקור (עדכן לפי הצורך)
SRCS = Game.cpp Player.cpp Governor.cpp Spy.cpp Baron.cpp General.cpp Judge.cpp Merchant.cpp

# הקבצים שירוצו
MAIN = Demo.cpp
TEST = Test.cpp

# שמות קבצי הפלט
EXEC = coup_game
TEST_EXEC = test_game

all: $(EXEC)

$(EXEC): $(SRCS) $(MAIN)
	$(CXX) $(CXXFLAGS) -o $(EXEC) $(SRCS) $(MAIN)

$(TEST_EXEC): $(SRCS) $(TEST)
	$(CXX) $(CXXFLAGS) -o $(TEST_EXEC) $(SRCS) $(TEST)

test: $(TEST_EXEC)
	./$(TEST_EXEC)

valgrind: $(TEST_EXEC)
	valgrind ./$(TEST_EXEC)

clean:
	rm -f *.o $(EXEC) $(TEST_EXEC)
