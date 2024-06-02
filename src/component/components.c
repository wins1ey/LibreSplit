#include "components.h"

LSComponent *ls_component_title_new();
LSComponent *ls_component_splits_new();
LSComponent *ls_component_timer_new();
LSComponent *ls_component_prev_segment_new();
LSComponent *ls_component_best_sum_new();
LSComponent *ls_component_pb_new();
LSComponent *ls_component_wr_new();

LSComponentAvailable ls_components[] =
{
    {"title",        ls_component_title_new},
    {"splits",       ls_component_splits_new},
    {"timer",        ls_component_timer_new},
    {"prev-segment", ls_component_prev_segment_new},
    {"best-sum",     ls_component_best_sum_new},
    {"pb",           ls_component_pb_new},
    {"wr",           ls_component_wr_new},
    {NULL, NULL}
};

void add_class(GtkWidget *widget, const char *class)
{
    gtk_style_context_add_class(gtk_widget_get_style_context(widget), class);
}

void remove_class(GtkWidget *widget, const char *class)
{
    gtk_style_context_remove_class(gtk_widget_get_style_context(widget), class);
}
