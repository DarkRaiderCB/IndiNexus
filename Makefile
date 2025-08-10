# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -Wall -g

# Directories
SRC_DIR = src
INC_DIR = include
BUILD_DIR = build
DATA_DIR = data

# Source files
SRCS = $(wildcard $(SRC_DIR)/*.cpp)
OBJS = $(patsubst $(SRC_DIR)/%.cpp,$(BUILD_DIR)/%.o,$(SRCS))
DEPS = $(OBJS:.o=.d)

# Include paths
INCLUDES = -I$(INC_DIR)

# Target executable
TARGET = $(BUILD_DIR)/IndiNexus

# Detect OS
ifeq ($(OS),Windows_NT)
    RM = rmdir /S /Q
    MKDIR = if not exist "$(1)" mkdir "$(1)"
    TARGET := $(TARGET).exe  # Add .exe extension on Windows
    SEP = \\
    LIBS = -lstdc++fs
else
    RM = rm -rf
    MKDIR = mkdir -p $(1)
    SEP = /
    LIBS =
endif

# Default rule
all: $(BUILD_DIR) $(TARGET)

# Ensure build directory exists
$(BUILD_DIR):
	$(call MKDIR,$(BUILD_DIR))

# Build target
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LIBS)

# Build object files with dependency tracking
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -MMD -MP -c $< -o $@

# Include dependency files
-include $(DEPS)

# Clean rule to remove all build artifacts
clean:
	-$(RM) $(BUILD_DIR)

# Phony targets
.PHONY: all clean
