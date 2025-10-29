# Themes

Create a theme stylesheet and place it in `themes/<name>/<name>.css` or
directly in `~/.config/libresplit/themes/<name>/<name>.css`, where `name` is the name of your theme.

You can set the global theme by changing the `theme` value in gsettings.

Theme variants should follow the pattern `<name>-<variant>.css`.
Each split file can apply their own themes by specifying a `theme` key in the main object.

See [this](https://docs.gtk.org/gtk3/css-properties.html) for a list of supported
CSS properties. Note that you can also modify the default font-family.

| LibreSplit CSS classes        | Explanation Where needed                        |
| ----------------------------- | ----------------------------------------------- |
| `.window`                     |                                                 |
| `.header`                     |                                                 |
| `.title`                      |                                                 |
| `.attempt-count`              |                                                 |
| `.time`                       |                                                 |
| `.delta`                      |                                                 |
| `.timer`                      |                                                 |
| `.timer-seconds`              |                                                 |
| `.timer-millis`               |                                                 |
| `.delay`                      | Timer not running/in negative time              |
| `.splits`                     | Container of the splits                         |
| `.split`                      | The splits themselves                           |
| `.current-split`              |                                                 |
| `.split-title`                |                                                 |
| `.split-icon`                 |                                                 |
| `.split-time`                 |                                                 |
| `.split-delta`                | Comparison time in the split                    |
| `.split-last`                 | The last split, if its not yet scrolled down to |
| `.done`                       |                                                 |
| `.behind`                     | Behind the PB but gaining time                  |
| `.losing`                     | Ahead of PB but losing time                     |
| `.behind.losing`              | (class combination) Behind PB and losing time   |
| `.best-segment`               |                                                 |
| `.best-split`                 |                                                 |
| `.footer`                     |                                                 |
| `.prev-segment-label`         |                                                 |
| `.prev-segment`               |                                                 |
| `.sum-of-bests-label`         |                                                 |
| `.sum-of-bests`               |                                                 |
| `.split-icon`                 |                                                 |
| `.personal-best-label`        |                                                 |
| `.personal-best`              |                                                 |
| `.world-record-label`         |                                                 |
| `.world-record`               |                                                 |

If a split has a `title` key, its UI element receives a class name derived from its title.

Specifically, the title is lowercase and all non-alphanumeric characters are replaced with hyphens, and the result is concatenated with `split-title-`.

For instance, if your split is titled "First split", it can be styled by targeting the CSS class `.split-title-first-split`.
