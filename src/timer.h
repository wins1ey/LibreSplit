#ifndef __TIMER_H__
#define __TIMER_H__

#define LAST_INFO_BEHIND_TIME   (1)
#define LAST_INFO_LOSING_TIME   (2)
#define LAST_INFO_BEST_SPLIT    (4)
#define LAST_INFO_BEST_SEGMENT  (8)

struct last_game
{
    char *path;
    char *title;
    char *theme;
    char *theme_variant;
    int attempt_count;
    int width;
    int height;
    long long world_record;
    long long start_delay;
    char **split_titles;
    int split_count;
    long long *split_times;
    long long *segment_times;
    long long *best_splits;
    long long *best_segments;
};
typedef struct last_game last_game;

struct last_timer
{
    int started;
    int running;
    int loading;
    long long now;
    long long start_time;
    long long time;
    long long sum_of_bests;
    long long world_record;
    int curr_split;
    long long *split_times;
    long long *split_deltas;
    long long *segment_times;
    long long *segment_deltas;
    int *split_info;
    long long *best_splits;
    long long *best_segments;
    const last_game *game;
  int *attempt_count;
};
typedef struct last_timer last_timer;

long long last_time_now(void);

long long last_time_value(const char *string);

void last_time_string(char *string, long long time);

void last_time_millis_string(char *seconds, char *millis, long long time);

void last_split_string(char *string, long long time);

void last_delta_string(char *string, long long time);

int last_game_create(last_game **game_ptr, const char *path);

void last_game_update_splits(last_game *game, const last_timer *timer);

void last_game_update_bests(last_game *game, const last_timer *timer);

int last_game_save(const last_game *game);

void last_game_release(last_game *game);

int last_timer_create(last_timer **timer_ptr, last_game *game);

void last_timer_release(last_timer *timer);

int last_timer_start(last_timer *timer);

void last_timer_step(last_timer *timer, long long now);

int last_timer_split(last_timer *timer);

int last_timer_skip(last_timer *timer);

int last_timer_unsplit(last_timer *timer);

void last_timer_stop(last_timer *timer);

int last_timer_reset(last_timer *timer);

int last_timer_cancel(last_timer *timer);

#endif /* __TIMER_H__ */
