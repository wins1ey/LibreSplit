INC = -I/usr/include/curl -I/usr/include/lua5.* -Iheaders
CFLAGS = -std=c++17 -O2 -pthread
LDFLAGS = -llua -lcurl

SRCDIR = ./src
BINDIR = ./bin
OBJDIR = $(BINDIR)/objects

# Obtain list of source files and create list of object files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES))
EXECUTABLE = $(BINDIR)/LAS

all: $(EXECUTABLE)

# Rule to link object files to create executable
$(EXECUTABLE): $(OBJECTS) | $(BINDIR)
	g++ $(CFLAGS) $(LDFLAGS) -o $@ $^

# Rule to compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	g++ $(INC) $(CFLAGS) -c -o $@ $<

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