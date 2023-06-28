INC = -I/usr/include/curl -I/usr/include/lua5.* `pkg-config --cflags gtk+-3.0 x11 jansson`
CFLAGS = -O2 -pthread -Wall -Wno-unused-parameter
LDFLAGS = -llua -lcurl -lstdc++fs `pkg-config --libs gtk+-3.0 x11 jansson`

SRCDIR = ./src
BINDIR = ./bin
OBJDIR = $(BINDIR)/objects

# Obtain list of source files and create list of object files
CPP_SOURCES = $(wildcard $(SRCDIR)/*.cpp)
C_SOURCES = $(wildcard $(SRCDIR)/*.c)
COMPONENTS = $(wildcard $(SRCDIR)/components/*.c)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(CPP_SOURCES)) \
          $(patsubst $(SRCDIR)/%.c, $(OBJDIR)/%.o, $(C_SOURCES)) \
          $(patsubst $(SRCDIR)/components/%.c, $(OBJDIR)/%.o, $(COMPONENTS))


EXECUTABLE = $(BINDIR)/LAST

all: $(EXECUTABLE)

# Rule to link object files to create executable
$(EXECUTABLE): $(OBJECTS) | $(BINDIR)
	g++ -std=c++17 $(CFLAGS) $(LDFLAGS) -o $@ $^

# Rule to compile C++ source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	g++ -std=c++17 $(INC) $(CFLAGS) -c -o $@ $<

# Rule to compile C source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	gcc -std=gnu99 $(INC) $(CFLAGS) -c -o $@ $<

# Rule to compile C component source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/components/%.c | $(OBJDIR)
	gcc -std=gnu99 $(INC) $(CFLAGS) -c -o $@ $<

# Rule to create the binary directory
$(BINDIR):
	mkdir -p $(BINDIR)

# Rule to create the object directory
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean target to remove object files and LAS executable
clean:
	rm -rf $(OBJDIR)
	rm -f $(EXECUTABLE)

# Clean target to remove the entire bin directory
clean-all: clean
	rm -rf $(BINDIR)

.PHONY: all clean clean-all