CXX 		:= g++
CPPFLAGS 	:= -Isrc
CXXFLAGS 	:= -std=c++20 -O2 -Wall -Wextra -Wpedantic -Werror
LDFLAGS 	:=
LDLIBS 		:=

TARGET 	:= build/RayTracing
SOURCES := $(wildcard src/*.cpp src/*/*.cpp)
OBJECTS := $(patsubst %.cpp,build/%.o,$(SOURCES))
DEPS 	:= $(OBJECTS:.o=.d)

.PHONY: all clean run
.DELETE_ON_ERROR:

all: $(TARGET)

$(TARGET): $(OBJECTS)
	$(CXX) $(LDFLAGS) $(OBJECTS) $(LDLIBS) -o $@

build/%.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf build

run: $(TARGET)
	$(TARGET)

-include $(DEPS)
