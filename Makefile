INC = -I/usr/include/curl -I./include
CFLAGS = -std=c++17 -O2 -pthread -I/usr/include/lua5.*
LDFLAGS = -llua -lcurl

SRCDIR = ./src
BINDIR = ./bin

# Obtain list of source files and create list of object files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(BINDIR)/%.o, $(SOURCES))
all: $(BINDIR)/LAS

# Rule to link object files to create executable
$(BINDIR)/LAS: $(OBJECTS) | $(BINDIR)
	g++ $(CFLAGS) $(LDFLAGS) -o $@ $^

# Rule to compile source files to object files
$(BINDIR)/%.o: $(SRCDIR)/%.cpp | $(BINDIR)
	g++ $(INC) $(CFLAGS) -c -o $@ $<

# Rule to create the object directory
$(BINDIR):
	mkdir -p $(BINDIR)

# Clean target to remove generated files
clean:
	rm -rf $(BINDIR)

.PHONY: all clean