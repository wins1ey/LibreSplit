#ifndef __AUTO_SPLITTER_H__
#define __AUTO_SPLITTER_H__

#include <linux/limits.h>
#include <stdatomic.h>

extern atomic_bool auto_splitter_enabled;
extern atomic_bool auto_splitter_running;
extern atomic_bool call_start;
extern atomic_bool call_split;
extern atomic_bool toggle_loading;
extern atomic_bool call_reset;
extern atomic_bool update_game_time;
extern atomic_llong game_time_value;
extern char auto_splitter_file[PATH_MAX];
extern int maps_cache_cycles_value;

void check_directories();
void run_auto_splitter();

#endif /* __AUTO_SPLITTER_H__ */
