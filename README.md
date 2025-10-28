<p align="center">
<img src="assets/libresplit.svg" width="100" height="100" align="top"/>
</p>
<h1 align="center"><a href="https://libresplit.loomeh.is-a.dev">LibreSplit</a></h1>

<p align="center">
<img src="https://img.shields.io/badge/Code-00599C?style=for-the-badge&logo=c&logoColor=white"</img>
<img src="https://img.shields.io/badge/AutoSplitters-000081?style=for-the-badge&logo=lua&logoColor=white"</img>
<img src="https://img.shields.io/github/stars/LibreSplit/LibreSplit?style=for-the-badge&logo=GitHub"</img>
<img src="https://img.shields.io/static/v1?label=Made%20with&message=GTK%203.0&color=725d9c&style=for-the-badge&logo=GTK&logoColor=white"/>
<img src="https://img.shields.io/github/license/LibreSplit/LibreSplit?label=license&style=for-the-badge&logo=GNU&logoColor=white&color=b85353"/>
<p>

<p align="center">
<a href="https://discord.gg/qbzD7MBjyw"><img src="https://img.shields.io/discord/1381914148585078804?style=for-the-badge&logo=Discord&label=LibreSplit&color=7289da"</img></a>
<a href="https://github.com/LibreSplit/LibreSplit/actions"><img src="https://img.shields.io/github/actions/workflow/status/LibreSplit/LibreSplit/build.yml?style=for-the-badge&logo=GitHub"</img></a>
</p>

## About

LibreSplit is a speedrun timer based on [urn](https://github.com/3snowp7im/urn) that adds support for Lua-based auto splitters that are easy to port from ASL.

<p align="center">
<img src="https://github.com/wins1ey/LibreSplit/assets/34382191/2adfdae5-9a21-4bdf-a4c4-f1d5962a0b63" width="350">
<img src="https://github.com/wins1ey/LibreSplit/assets/34382191/4455f57a-3d34-4fa3-9dff-2b342b6c56da" width="350">
</p>

### If you are looking for the public repository of splits, autosplitters and themes. They are located [here](https://github.com/LibreSplit/LibreSplit-resources)

## Features

- **Split Tracking and Timing:** Accurately track and time your speedruns with ease.
- **Auto Splitter Support:** Utilize Lua-based auto splitters to automate split timing based on in-game events.
- **Customizable Themes:** Personalize your timer's appearance by creating and applying custom themes.
- **Flexible Configuration:** Configure keybindings and various settings to suit your preferences.
- **Icon support for splits.**
- **Always on Top support.**
- **Auto Splitter Support:** Use Lua to write your own auto splitters or easily port them from LiveSplit's ASL.

----

## Quick Start and Installation

### Install using your distro's package manager (recommended)

<details>
<summary>Arch-based Distros</summary>

See the [libresplit-git](https://aur.archlinux.org/packages/libresplit-git) package on the Arch User Repository (AUR).

If you're using an AUR helper (like `yay`) you can use it to install LibreSplit, for example:

```sh
yay libresplit-git
```
</details>

<details>
<summary>NixOS</summary>

See the [libresplit](https://search.nixos.org/packages?channel=25.05&show=libresplit&query=libresplit) package, courtesy of [@fgaz](https://github.com/fgaz).
</details>

### Building Manually

LibreSplit requires the following dependencies on your system to compile:

- `meson`
- `libgtk+-3.0`
- `x11`
- `libjansson`
- `luajit`

Install the required dependencies:

<details>
<summary>Debian-based systems</summary>

```sh
sudo apt update
sudo apt install build-essential libgtk3-dev libjansson-dev meson luajit
```
</details>

<details>
<summary>Arch-based systems</summary>

```sh
sudo pacman -Syu
sudo pacman -S gtk3 jansson luajit git meson
```
</details>

Clone the project:

```sh
git clone https://github.com/LibreSplit/LibreSplit
cd LibreSplit
```

Now compile and install:

```sh
meson setup build -Dbuildtype=release
meson compile -C build
meson install -C build
```

All done! Now you can start the desktop **LibreSplit** or run `/usr/local/bin/libresplit`.

----

<!-- TODO: Finish -->

## Getting Started

1. Launch LibreSplit by executing the compiled binary. `libresplit` inside build
2. When first launched, LibreSplit will create the `libresplit` directory in your config directory. Auto splitters, splits and themes go in their respective folders inside.
3. The initial window is undecorated, but you can toggle window decorations by pressing the right Control key.
4. Control the timer using the following key presses:

   | Key        | Stopped Action | Started Action |
   |------------|----------------|----------------|
   | Spacebar   | Start          | Split          |
   | Backspace  | Reset          | Stop           |
   | Delete     | Cancel         | -              |

- The "Cancel" action resets the timer and decrements the attempt counter. A run reset before the start delay is automatically cancelled.
- To manually modify the current split, use the following key actions:

   | Key         | Action        |
   |-------------|---------------|
   | Page Up     | Unsplit       |
   | Page Down   | Skip split    |

4. Customize keybindings by setting the values in `com.github.wins1ey.libresplit` path with `gsettings`.

   | Key                        | Type    | Description                       |
   |----------------------------|---------|-----------------------------------|
   | start-decorated            | Boolean | Start with window decorations     |
   | hide-cursor                | Boolean | Hide cursor in window             |
   | global-hotkeys             | Boolean | Enables global hotkeys            |
   | theme                      | String  | Default theme name                |
   | theme-variant              | String  | Default theme variant             |
   | keybind-start-split        | String  | Start/split keybind               |
   | keybind-stop-reset         | String  | Stop/Reset keybind                |
   | keybind-cancel             | String  | Cancel keybind                    |
   | keybind-unsplit            | String  | Unsplit keybind                   |
   | keybind-skip-split         | String  | Skip split keybind                |
   | keybind-toggle-decorations | String  | Toggle window decorations keybind |

Keybind strings should be parseable by
[gtk_accelerator_parse](https://developer.gnome.org/gtk3/stable/gtk3-Keyboard-Accelerators.html#gtk-accelerator-parse).

For more information: [here](https://docs.gtk.org/gtk4/func.accelerator_parse.html) and [here](https://gitlab.gnome.org/GNOME/gtk/-/blob/main/gdk/gdkkeysyms.h)

## Auto Splitters

LibreSplit supports auto splitters written in Lua to automate split timing based on in-game events. Feel free to make your own, Documentation can be found [here](docs/auto-splitters.md)

## Split Files

Split files in LibreSplit are stored as well-formed JSON. The split file must contain a main object. The following keys are optional:

| Key           | Value                                 |
|---------------|---------------------------------------|
| title         | Title string at top of window         |
| start_delay   | Non-negative delay until timer starts |
| world_record  | Best known time                       |
| splits        | Array of split objects                |
| theme         | Window theme                          |
| theme_variant | Window theme variant                  |
| width         | Window width                          |
| height        | Window height                         |

Each split object within the `splits` array has the following keys:

| Key          | Value                  |
|--------------|------------------------|
| title        | Split title            |
| time         | Split time             |
| best_time    | Your best split time   |
| best_segment | Your best segment time |

Times are strings in HH:MM:SS.mmmmmm format

## Themes

LibreSplit supports customizable themes, allowing you to personalize the timer's appearance. To create a theme:

1. Create a CSS stylesheet with your desired styles.
2. Place the stylesheet in the `~/.config/libresplit/themes/<name>/<name>.css` directory. (If you have `XDG_CONFIG_HOME` env var pointing somewher else than .config it will be wherever it points to)
3. Set the global theme by modifying the `theme` value in `gsettings`.
4. Theme variants should follow the pattern `<name>-<variant>.css`.
5. Individual splits can apply their own themes by specifying a `theme` key in the main split object.

For a list of supported CSS properties, refer to the [GtkCssProvider](https://developer.gnome.org/gtk3/stable/GtkCssProvider.html) documentation.

## CSS Classes

The following CSS classes can be used to style the elements of the LibreSplit interface:

```css
.window
.header
.title
.attempt-count
.time
.delta
.timer
.timer-seconds
.timer-millis
.delay
.splits
.split
.current-split
.split-title
.split-time
.split-delta
.split-last
.done
.behind
.losing
.best-segment
.best-split
.footer
.prev-segment-label
.prev-segment
.sum-of-bests-label
.sum-of-bests
.personal-best-label
.personal-best
.world-record-label
.world-record
