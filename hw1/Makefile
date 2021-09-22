.PHONY: all clean run
all: # The default target

# tool macros
CC := g++
CXXFLAGS := -Wall -std=c++17

# compile macros
TARGET := main

# non-phony targets
$(TARGET): main.o

# phony rules
all: $(TARGET)

clean:
	rm -rf core.* *.o $(TARGET)

run: $(TARGET)
	@./$(TARGET)
