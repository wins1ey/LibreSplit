#ifndef __COMPONENTS_H__
#define __COMPONENTS_H__

#include <ctype.h>
#include <gtk/gtk.h>
#include <stdlib.h>
#include <string.h>

#include "../timer.h"

typedef struct _LSComponent LSComponent;
typedef struct _LSComponentOps LSComponentOps;
typedef struct _LSComponentAvailable LSComponentAvailable;

struct _LSComponent {
    LSComponentOps* ops;
};

struct _LSComponentOps {
    void (*delete)(LSComponent* self);
    GtkWidget* (*widget)(LSComponent* self);

    void (*resize)(LSComponent* self, int win_width, int win_height);
    void (*show_game)(LSComponent* self, ls_game* game, ls_timer* timer);
    void (*clear_game)(LSComponent* self);
    void (*draw)(LSComponent* self, ls_game* game, ls_timer* timer);

    void (*start_split)(LSComponent* self, ls_timer* timer);
    void (*skip)(LSComponent* self, ls_timer* timer);
    void (*unsplit)(LSComponent* self, ls_timer* timer);
    void (*stop_reset)(LSComponent* self, ls_timer* timer);
    void (*cancel_run)(LSComponent* self, ls_timer* timer);
};

struct _LSComponentAvailable {
    char* name;
    LSComponent* (*new)();
};

// A NULL-terminated array of all available components
extern LSComponentAvailable ls_components[];

// Utility functions
void add_class(GtkWidget* widget, const char* class);

void remove_class(GtkWidget* widget, const char* class);

#endif /* __COMPONENTS_H__ */
