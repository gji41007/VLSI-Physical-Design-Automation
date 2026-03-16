
CXX = g++
CXXFLAGS = -std=c++14 -O2 -Wall -Wextra
LDFLAGS = 


SRCDIR = .
OBJDIR = obj
BINDIR = ../bin


CPP_SOURCES = $(wildcard *.cpp)
OBJECTS = $(CPP_SOURCES:%.cpp=$(OBJDIR)/%.o)
TARGET = $(BINDIR)/hw4
TESTCASE = ibm01.modified

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
	$(TARGET) ../testcase/${TESTCASE}.txt ../output/${TESTCASE}.out


clean:
	@rm -rf $(OBJDIR)
	@rm -f $(TARGET)
	@echo "Clean completed"


.PHONY: all run clean 