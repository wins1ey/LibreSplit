#include "components.h"

typedef struct _LSWr {
    LSComponent base;
    GtkWidget* container;
    GtkWidget* world_record_label;
    GtkWidget* world_record;
} LSWr;
extern LSComponentOps ls_wr_operations;

#define WORLD_RECORD "World record"

LSComponent* ls_component_wr_new()
{
    LSWr* self;

    self = malloc(sizeof(LSWr));
    if (!self) {
        return NULL;
    }
    self->base.ops = &ls_wr_operations;

    self->container = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    add_class(self->container, "footer"); /* hack */
    gtk_widget_show(self->container);

    self->world_record_label = gtk_label_new(WORLD_RECORD);
    add_class(self->world_record_label, "world-record-label");
    gtk_container_add(GTK_CONTAINER(self->container), self->world_record_label);

    self->world_record = gtk_label_new(NULL);
    add_class(self->world_record, "world-record");
    add_class(self->world_record, "time");
    gtk_widget_set_halign(self->world_record, GTK_ALIGN_END);
    gtk_container_add(GTK_CONTAINER(self->container), self->world_record);

    return (LSComponent*)self;
}

static void wr_delete(LSComponent* self)
{
    free(self);
}

static GtkWidget* wr_widget(LSComponent* self)
{
    return ((LSWr*)self)->container;
}

static void wr_show_game(LSComponent* self_,
    ls_game* game, ls_timer* timer)
{
    LSWr* self = (LSWr*)self_;
    gtk_widget_set_halign(self->world_record_label, GTK_ALIGN_START);
    gtk_widget_set_hexpand(self->world_record_label, TRUE);
    if (game->world_record) {
        char str[256];
        ls_time_string(str, game->world_record);
        gtk_label_set_text(GTK_LABEL(self->world_record), str);
        gtk_widget_show(self->world_record);
        gtk_widget_show(self->world_record_label);
    }
}

static void wr_clear_game(LSComponent* self_)
{
    LSWr* self = (LSWr*)self_;
    gtk_widget_hide(self->world_record_label);
    gtk_widget_hide(self->world_record);
}

static void wr_draw(LSComponent* self_, ls_game* game,
    ls_timer* timer)
{
    LSWr* self = (LSWr*)self_;
    char str[256];
    if (timer->curr_split == game->split_count
        && game->world_record) {
        if (timer->split_times[game->split_count - 1]
            && timer->split_times[game->split_count - 1]
                < game->world_record) {
            ls_time_string(str, timer->split_times[game->split_count - 1]);
        } else {
            ls_time_string(str, game->world_record);
        }
        gtk_label_set_text(GTK_LABEL(self->world_record), str);
    }
}

LSComponentOps ls_wr_operations = {
    .delete = wr_delete,
    .widget = wr_widget,
    .show_game = wr_show_game,
    .clear_game = wr_clear_game,
    .draw = wr_draw
};
