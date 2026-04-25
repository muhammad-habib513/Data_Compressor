CXX := g++
CXXFLAGS := -std=c++17 -O3 -DNDEBUG -Wall -Wextra -pedantic
TARGET := compressor
SRC := src/main.cpp src/pipeline.cpp src/rle1.cpp src/bwt.cpp
OBJ := $(SRC:.cpp=.o)
TEST_TARGET := tests
TEST_SRC := src/tests.cpp src/pipeline.cpp src/rle1.cpp src/bwt.cpp
TEST_OBJ := $(TEST_SRC:.cpp=.o)

.PHONY: all clean run-example test

all: $(TARGET) $(TEST_TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

src/%.o: src/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(TEST_TARGET): $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_OBJ)

clean:
	rm -f $(OBJ) $(TEST_OBJ) $(TARGET) $(TEST_TARGET)

run-example: $(TARGET)
	./$(TARGET) encode sample.txt sample.dcp 200
	./$(TARGET) decode sample.dcp sample_out.txt

test: $(TEST_TARGET)
	./$(TEST_TARGET)
