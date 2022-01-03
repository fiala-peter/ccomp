TARGET = ccomp

CXX = g++
CFLAGS = -c -g -pedantic -std=c++17
LFLAGS =

SRCDIR   = src
OBJDIR   = obj
BINDIR   = bin
TESTDIR   = test_inputs

SOURCES  := $(wildcard $(SRCDIR)/*.cpp)
INCLUDES := $(wildcard $(SRCDIR)/*.h)
OBJECTS  := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.o)
DEPS  	 := $(SOURCES:$(SRCDIR)/%.cpp=$(OBJDIR)/%.d)
TEST_SOURCES := $(wildcard $(TESTDIR)/*.c)
TEST_ASMS    := $(TEST_SOURCES:$(TESTDIR)/%.c=$(TESTDIR)/%.s)
TEST_BINS    := $(TEST_SOURCES:$(TESTDIR)/%.c=$(TESTDIR)/%.out)

$(BINDIR)/$(TARGET): $(OBJECTS)
	$(CXX) $(OBJECTS) $(LFLAGS) -o $@

$(OBJECTS): $(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CXX) $(CFLAGS) -MMD $< -o $@

include $(DEPS)

$(DEPS): ;

.PHONY: clean
clean:
	rm -f $(OBJECTS) $(BINDIR)/$(TARGET) $(DEPS) $(TEST_ASMS) $(TEST_BINS)

.PHONY: doc
doc: $(SOURCES) $(INCLUDES)
	doxygen

$(TEST_ASMS): %.s : %.c
	bin/ccomp $< -o $@

$(TEST_BINS): %.out : %.s
	gcc $< -o $@ -no-pie

test: $(TEST_BINS)
