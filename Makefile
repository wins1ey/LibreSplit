INC = -I/usr/include/curl -I./include
CFLAGS = -std=c++17 -O2 -pthread -I/usr/include/lua5.*
LDFLAGS = -llua -lcurl

SRCDIR = ./src
OBJDIR = ./obj
BUILD = ./build
# Obtain list of source files and create list of object files
SOURCES = $(wildcard $(SRCDIR)/*.cpp)
OBJECTS = $(patsubst $(SRCDIR)/%.cpp, $(OBJDIR)/%.o, $(SOURCES))
all: $(BUILD)/LAS

# Rule to link object files to create executable
$(BUILD)/LAS: $(OBJECTS) | $(BUILD)
	g++ $(CFLAGS) $(LDFLAGS) -o $@ $^

# Rule to compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.cpp | $(OBJDIR)
	g++ $(INC) $(CFLAGS) -c -o $@ $<

# Rule to create the build directory
$(BUILD):
	mkdir -p $(BUILD)

# Rule to create the object directory
$(OBJDIR):
	mkdir -p $(OBJDIR)

# Clean target to remove generated files
clean:
	rm -rf $(OBJDIR) $(BUILD)

.PHONY: all clean