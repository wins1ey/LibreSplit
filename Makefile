BIN = $(BIN_DIR)/LAST

INC = -I/usr/include/curl -I/usr/include/lua5.* `pkg-config --cflags gtk+-3.0 x11 jansson`
CFLAGS = -O2 -pthread -Wall -Wno-unused-parameter
LDFLAGS = -llua -lcurl -lstdc++fs `pkg-config --libs gtk+-3.0 x11 jansson`

SRC_DIR = ./src
BIN_DIR = ./bin
OBJ_DIR = $(BIN_DIR)/objects
HEADERS_DIR = $(SRC_DIR)/headers

# Obtain list of source files and create list of object files
CPP_SOURCES = $(wildcard $(SRC_DIR)/*.cpp)
C_SOURCES = $(wildcard $(SRC_DIR)/*.c)
COMPONENTS = $(wildcard $(SRC_DIR)/components/*.c)
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(CPP_SOURCES)) \
          $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(C_SOURCES)) \
          $(patsubst $(SRC_DIR)/components/%.c, $(OBJ_DIR)/%.o, $(COMPONENTS))

USR_BIN_DIR = /usr/local/bin
APP = last.desktop
APP_DIR = /usr/share/applications
ICON = last
ICON_DIR = /usr/share/icons/hicolor
SCHEMAS_DIR = /usr/share/glib-2.0/schemas

all: $(BIN)

$(OBJECTS): last-css.h

last-css.h: last.css
	xxd --include last.css > $(HEADERS_DIR)/last-css.h || (rm $(HEADERS_DIR)/last-css.h; false)

# Rule to link object files to create executable
$(BIN): $(OBJECTS) | $(BIN_DIR)
	g++ -std=c++17 $(CFLAGS) $(LDFLAGS) -o $@ $^

# Rule to compile C++ source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	g++ -std=c++17 $(INC) $(CFLAGS) -c -o $@ $<

# Rule to compile C source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	gcc -std=gnu99 $(INC) $(CFLAGS) -c -o $@ $<

# Rule to compile C component source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/components/%.c | $(OBJ_DIR)
	gcc -std=gnu99 $(INC) $(CFLAGS) -c -o $@ $<

# Rule to create the binary directory
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Rule to create the object directory
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

install:
	cp $(BIN) $(USR_BIN_DIR)
	cp $(APP) $(APP_DIR)
	for size in 16 22 24 32 36 48 64 72 96 128 256 512; do \
	  convert $(ICON).png -resize "$$size"x"$$size" \
	          $(ICON_DIR)/"$$size"x"$$size"/apps/$(ICON).png ; \
	done
	gtk-update-icon-cache -f -t $(ICON_DIR)
	cp last-gtk.gschema.xml $(SCHEMAS_DIR)
	glib-compile-schemas $(SCHEMAS_DIR)
	mkdir -p /usr/share/last/themes
	rsync -a --exclude=".*" themes /usr/share/last

uninstall:
	rm -f $(USR_BIN_DIR)/LAST
	rm -f $(APP_DIR)/$(APP)
	rm -rf /usr/share/last
	for size in 16 22 24 32 36 48 64 72 96 128 256 512; do \
	  rm -f $(ICON_DIR)/"$$size"x"$$size"/apps/$(ICON).png ; \
	done

remove-schema:
	rm -f $(SCHEMAS_DIR)/last-gtk.gschema.xml
	glib-compile-schemas $(SCHEMAS_DIR)

# Clean target to remove object files and LAS executable
clean:
	rm -rf $(BIN_DIR) $(HEADERS_DIR)/last-css.h

.PHONY: all install uninstall remove-schema clean clean-all