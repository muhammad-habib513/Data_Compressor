CXX := g++
CXXFLAGS := -std=c++17 -O3 -DNDEBUG -Wall -Wextra -pedantic

TARGET := compressor

SRC := src/main.cpp \
       src/pipeline.cpp \
       src/rle1.cpp \
       src/bwt.cpp \
       src/mtf.cpp \
       src/rle2.cpp \
       src/ans.cpp

OBJ := $(SRC:.cpp=.o)

TEST_TARGET := tests

TEST_SRC := src/tests.cpp \
            src/pipeline.cpp \
            src/rle1.cpp \
            src/bwt.cpp \
            src/mtf.cpp \
            src/rle2.cpp \
            src/ans.cpp

TEST_OBJ := $(TEST_SRC:.cpp=.o)

.PHONY: all clean test

all: $(TARGET) $(TEST_TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(OBJ)

$(TEST_TARGET): $(TEST_OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $(TEST_OBJ)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

test: $(TEST_TARGET)
	./$(TEST_TARGET)

clean:
	rm -f $(OBJ) $(TEST_OBJ) $(TARGET) $(TEST_TARGET)
