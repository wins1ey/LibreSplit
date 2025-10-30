# Settings and keybinds

LibreSplit uses the GSettings schema as its way to keep track of your preferences.

## The settings schema

The settings schema is defined in the [com.github.wins1ey.libresplit.gschema.xml](https://github.com/LibreSplit/LibreSplit/assets/com.github.wins1ey.libresplit.gschema.xml) file.

| Setting           | Type    | Description                        | Default        |
| ----------------- | ------- | ---------------------------------- | -------------- |
| `start-decorated` | Boolean | Start with window decorations      | `false`        |
| `start-on-top`    | Boolean | Start with window as always on top | `true`         |
| `hide-cursor`     | Boolean | Hide cursor in window              | `false`        |
| `global-hotkeys`  | Boolean | Enables global hotkeys             | `false`        |
| `start-on-top`    | Boolean | Start with window as always on top | `false`        |
| `theme`           | String  | Default theme name                 | `'standard'`   |
| `theme-variant`   | String  | Default theme variant              | `''`           |

| Keybind                      | Type   | Description                                 | Default                     |
| ---------------------------- | ------ | ---------------------------------           | ---------------------       |
| `keybind-start-split`        | String | Start/split keybind                         | <kbd>Space</kbd>            |
| `keybind-stop-reset`         | String | Stop/Reset keybind                          | <kbd>Backspace</kbd>        |
| `keybind-cancel`             | String | Cancel keybind                              | <kbd>Delete</kbd>           |
| `keybind-unsplit`            | String | Unsplit keybind                             | <kbd>Page Up</kbd>          |
| `keybind-skip-split`         | String | Skip split keybind                          | <kbd>Page Down</kbd>        |
| `keybind-toggle-decorations` | String | Toggle window decorations keybind           | <kbd>Right Ctrl</kbd>       |
| `keybind-toggle-win-on-top`  | String | Toggle window "Always on top" state keybind | <kbd>CTRL</kbd><kbd>k</kbd> |

## Modifying the default values

You can change the values in the `com.github.wins1ey.libresplit` path using `gsettings`:

```sh
# Enabling the global hotkeys
gsettings set com.github.wins1ey.libresplit global-hotkeys true

# Changing the theme
gsettings set com.github.wins1ey.libresplit theme <my-theme>

# Change the keybind to start/split
gsettings set com.github.wins1ey.libresplit keybind-start-split "<Alt>space"
```

You can also directly edit the [com.github.wins1ey.libresplit.gschema.xml](https://github.com/LibreSplit/LibreSplit/assets/com.github.wins1ey.libresplit.gschema.xml) file, but you will need to use `meson` again to get the required `com.github.wins1ey.libresplit.gschema.xml` into the expected location.

Keybind strings must be parsable by the
[gtk_accelerator_parse](https://docs.gtk.org/gtk4/func.accelerator_parse.html).
See the [complete list of keynames](https://github.com/GNOME/gtk/blob/main/gdk/keynames.txt) for `gdk`. Modifiers are enclosed in angular brackets <>: `<Shift>`, `<Ctrl>`, `<Alt>`, `<Meta>`, `<Super>`, `<Hyper>`. Note that you should use `<Alt>a` instead of `<Alt>-a` or similar.
