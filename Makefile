BIN := libresplit

INC := `pkg-config --cflags gtk+-3.0 x11 jansson luajit`
CFLAGS := -std=gnu99 -O2 -pthread -Wall -Wno-unused-parameter
LDFLAGS := `pkg-config --libs gtk+-3.0 x11 jansson luajit`

SRC_DIR := ./src
OBJ_DIR := ./obj

# Obtain list of source files and create list of object files
SOURCES := $(wildcard $(SRC_DIR)/*.c)
COMPONENTS := $(wildcard $(SRC_DIR)/component/*.c)
OBJECTS := $(patsubst $(SRC_DIR)/%.c, $(OBJ_DIR)/%.o, $(SOURCES)) \
          $(patsubst $(SRC_DIR)/component/%.c, $(OBJ_DIR)/%.o, $(COMPONENTS))

DESTDIR :=
PREFIX := /usr/local
APP := libresplit.desktop
ICON := libresplit
SCHEMA := libresplit.gschema.xml

ifdef DESTDIR
	update_icon_cache :=
	compile_schemas :=
else
	update_icon_cache := gtk-update-icon-cache -f -t $(PREFIX)/share/icons/hicolor
	compile_schemas := glib-compile-schemas $(PREFIX)/share/glib-2.0/schemas
endif

all: main.h $(BIN)

# Rule to link object files to create executable
$(BIN): $(OBJECTS)
	gcc $(CFLAGS) $^ $(LDFLAGS) -o $@

# Rule to compile C source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.c | $(OBJ_DIR)
	gcc $(INC) $(CFLAGS) -c -o $@ $<

# Rule to compile C component source files to object files
$(OBJ_DIR)/%.o: $(SRC_DIR)/component/%.c | $(OBJ_DIR)
	gcc $(INC) $(CFLAGS) -c -o $@ $<

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

main.h: $(SRC_DIR)/main.css
	xxd --include $(SRC_DIR)/main.css > $(SRC_DIR)/main.h || ($(RM) $(SRC_DIR)/main.h; false)

install: all
	install -Dm755 $(BIN) $(DESTDIR)$(PREFIX)/bin/$(BIN)
	install -Dm644 $(APP) $(DESTDIR)$(PREFIX)/share/applications/$(APP)
	for size in 16 22 24 32 36 48 64 72 96 128 256 512; do \
		mkdir -p $(DESTDIR)$(PREFIX)/share/icons/hicolor/"$$size"x"$$size"/apps ; \
		rsvg-convert -w "$$size" -h "$$size" -f png -o $(DESTDIR)$(PREFIX)/share/icons/hicolor/"$$size"x"$$size"/apps/$(ICON).png $(ICON).svg ; \
	done
	$(update_icon_cache)
	install -Dm644 $(SRC_DIR)/$(SCHEMA) $(DESTDIR)$(PREFIX)/share/glib-2.0/schemas/$(SCHEMA)
	$(compile_schemas)
	install -Dm644 resources/themes/standard/standard.css $(DESTDIR)$(PREFIX)/share/libresplit/themes/standard/standard.css
uninstall:
	$(RM) $(DESTDIR)$(PREFIX)/bin/$(BIN)
	$(RM) $(DESTDIR)$(PREFIX)/share/applications/$(APP)
	$(RM) -r $(DESTDIR)$(PREFIX)/share/libresplit
	for size in 16 22 24 32 36 48 64 72 96 128 256 512; do \
		$(RM) $(DESTDIR)$(PREFIX)/share/icons/hicolor/"$$size"x"$$size"/apps/$(ICON).png ; \
	done

remove-schema:
	$(RM) $(DESTDIR)$(PREFIX)/share/glib-2.0/schemas/$(SCHEMA)
	$(compile_schemas)

clean:
	$(RM) -r $(BIN) $(OBJ_DIR) $(SRC_DIR)/main.h

.PHONY: all main.h install uninstall remove-schema clean
