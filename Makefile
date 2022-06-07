#Toolchain
CXX := g++
LD := $(CXX)
PROJSRCDIR := ./src

# Compiler and Linker flags
INCLUDE_DIRS		:= ./src
LINKER_INPUTS		:= -lpthread -lrt -Wl,--gc-sections

#CFLAGS := -g -ggdb -ffunction-sections -O0
CXXFLAGS := -DLINBUILD -fdata-sections -ffunction-sections -O0 -std=c++11 -Wall -Wno-format -Wno-unused-but-set-variable -Wno-unused-variable -Wno-unused-local-typedefs
CXXFLAGS += $(addprefix -I,$(INCLUDE_DIRS))
VPATH=src/

# Source file
SOURCES = src/tsa_app_main.cpp \
src/tsa_null_audio.cpp \
src/tsa_parser.cpp \
src/tsa_ts_info.cpp

# Object file
OBJ = $(SOURCES:.cpp=.o)

# Output binary name
PROGRAM = tsanalyzer

all: $(PROGRAM)

$(PROGRAM): $(OBJ)
		$(CXX) -o ./bin/$(PROGRAM) $(OBJ) -L$(LINKER_INPUTS)

.SUFFIXES = .cpp

.cpp.o:
		$(CXX) $(INCLUDE) $(CXXFLAGS) -c $< -o $@

clean:
		rm  ./src/*.o \
			./bin/$(PROGRAM)

purge:
		rm -r ./src/

