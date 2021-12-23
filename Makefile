TARGET = ccomp

CXX = g++
CFLAGS = -c -g -pedantic -std=c++17
LFLAGS =

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin

SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
DEPS  	 := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.d)

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LFLAGS) -o $@

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CXX) $(CFLAGS) -MMD $< -o $@

include $(DEPS)

$(DEPS): ;

.PHONY: clean
clean:
	rm -f $(OBJECTS) $(BINDIR)/$(TARGET) $(DEPS) test_inputs/*.s test_inputs/*.lex test_inputs/*.ast test_inputs/*.i

.PHONY: doc
doc: $(SOURCES) $(INCLUDES)
	doxygen
