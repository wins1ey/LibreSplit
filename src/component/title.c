#include "components.h"

typedef struct _LSTitle {
    LSComponent base;
    GtkWidget* header;
    GtkWidget* title;
    GtkWidget* attempt_count;
    GtkWidget* finished_count;
} LSTitle;
extern LSComponentOps ls_title_operations; // defined at the end of the file

LSComponent* ls_component_title_new()
{
    LSTitle* self;

    self = malloc(sizeof(LSTitle));
    if (!self) {
        return NULL;
    }
    self->base.ops = &ls_title_operations;

    self->header = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    add_class(self->header, "header");
    gtk_widget_show(self->header);

    self->title = gtk_label_new(NULL);
    add_class(self->title, "title");
    gtk_label_set_justify(GTK_LABEL(self->title), GTK_JUSTIFY_CENTER);
    gtk_label_set_line_wrap(GTK_LABEL(self->title), TRUE);
    gtk_widget_set_hexpand(self->title, TRUE);
    gtk_container_add(GTK_CONTAINER(self->header), self->title);

    self->attempt_count = gtk_label_new(NULL);
    add_class(self->attempt_count, "attempt-count");
    gtk_widget_set_margin_start(self->attempt_count, 8);
    gtk_widget_set_valign(self->attempt_count, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(self->header), self->attempt_count);
    gtk_widget_show(self->attempt_count);

    self->finished_count = gtk_label_new(NULL);
    add_class(self->finished_count, "finished_count");
    gtk_widget_set_margin_start(self->finished_count, 8);
    gtk_widget_set_valign(self->finished_count, GTK_ALIGN_START);
    gtk_container_add(GTK_CONTAINER(self->header), self->finished_count);
    gtk_widget_show(self->finished_count);

    return (LSComponent*)self;
}

static void title_delete(LSComponent* self)
{
    free(self);
}

static GtkWidget* title_widget(LSComponent* self)
{
    return ((LSTitle*)self)->header;
}

static void title_resize(LSComponent* self_, int win_width, int win_height)
{
    GdkRectangle rect;
    int attempt_count_width;
    int finished_count_width;
    int title_width;
    LSTitle* self = (LSTitle*)self_;

    gtk_widget_hide(self->title);
    gtk_widget_get_allocation(self->attempt_count, &rect);
    attempt_count_width = rect.width;
    gtk_widget_get_allocation(self->finished_count, &rect);
    finished_count_width = rect.width;
    title_width = win_width - (attempt_count_width + finished_count_width);
    rect.width = title_width;
    gtk_widget_show(self->title);
    gtk_widget_set_allocation(self->title, &rect);
}

static void title_show_game(LSComponent* self_, ls_game* game,
    ls_timer* timer)
{
    char str[64];
    LSTitle* self = (LSTitle*)self_;
    gtk_label_set_text(GTK_LABEL(self->title), game->title);
    sprintf(str, "#%d", game->attempt_count);
    gtk_label_set_text(GTK_LABEL(self->attempt_count), str);
}

static void title_draw(LSComponent* self_, ls_game* game, ls_timer* timer)
{
    char attempt_str[64];
    char finished_str[64];
    char combi_str[64];
    LSTitle* self = (LSTitle*)self_;
    sprintf(attempt_str, "%d", game->attempt_count);
    sprintf(finished_str, "#%d", game->finished_count);
    strcpy(combi_str, finished_str);
    strcat(combi_str, "/");
    strcat(combi_str, attempt_str);
    gtk_label_set_text(GTK_LABEL(self->attempt_count), combi_str);
}

LSComponentOps ls_title_operations = {
    .delete = title_delete,
    .widget = title_widget,
    .resize = title_resize,
    .show_game = title_show_game,
    .draw = title_draw
};
