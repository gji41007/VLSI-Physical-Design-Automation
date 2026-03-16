# Compiler and flags
CXX = g++
CXXFLAGS = -std=c++17 -O2 -Wall -Wextra
LDFLAGS = 

# Directories
SRCDIR = .
OBJDIR = obj
BINDIR = ../bin

# Source files
CPP_SOURCES = $(wildcard *.cpp)
OBJECTS = $(CPP_SOURCES:%.cpp=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/hw3
TESTCASE = public4

# Default target
all: $(TARGET)

# Create target executable
$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build completed: $(TARGET)"

# Compile source files to object files
$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Create directories
$(OBJDIR):
	@mkdir -p $@

$(BINDIR):
	@mkdir -p $@

# Run the program with example arguments
run: $(TARGET)
	@echo "Running detailed placer..."
	$(TARGET) ../testcase/${TESTCASE}.lef ../testcase/${TESTCASE}.def ../output/${TESTCASE}.def

# Clean build artifacts
clean:
	@rm -rf $(OBJDIR)
	@rm -f $(TARGET)
	@echo "Clean completed"

# Debug build
debug: CXXFLAGS += -g -DDEBUG
debug: $(TARGET)

# Install dependencies (if any)
deps:
	@echo "Checking dependencies..."
	@which $(CXX) > /dev/null || (echo "Error: $(CXX) not found" && exit 1)

# Show help
help:
	@echo "Available targets:"
	@echo "  all     - Build the project (default)"
	@echo "  run     - Build and run with example testcase"
	@echo "  clean   - Remove all build artifacts"
	@echo "  debug   - Build with debug flags"
	@echo "  deps    - Check build dependencies"

.PHONY: all run clean debug deps help