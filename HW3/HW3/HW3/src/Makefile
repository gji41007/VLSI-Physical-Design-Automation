
CXX = g++
CXXFLAGS = -std=c++14 -O2 -Wall -Wextra
LDFLAGS = 


SRCDIR = .
OBJDIR = obj
BINDIR = ../bin


CPP_SOURCES = $(wildcard *.cpp)
OBJECTS = $(CPP_SOURCES:%.cpp=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/hw3
TESTCASE = public4

all: $(TARGET)

$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CXX) $(OBJECTS) -o $@ $(LDFLAGS)
	@echo "Build completed: $(TARGET)"


$(OBJDIR)/%.o: %.cpp | $(OBJDIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@


$(OBJDIR):
	@mkdir -p $@

$(BINDIR):
	@mkdir -p $@


run: $(TARGET)
	@echo "Running detailed placer..."
	$(TARGET) ../testcase/${TESTCASE}.lef ../testcase/${TESTCASE}.def ../output/${TESTCASE}.def


clean:
	@rm -rf $(OBJDIR)
	@rm -f $(TARGET)
	@echo "Clean completed"


.PHONY: all run clean 