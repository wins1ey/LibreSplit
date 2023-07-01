#include "last-component.h"

LASTComponent *last_component_title_new();
LASTComponent *last_component_splits_new();
LASTComponent *last_component_timer_new();
LASTComponent *last_component_prev_segment_new();
LASTComponent *last_component_best_sum_new();
LASTComponent *last_component_pb_new();
LASTComponent *last_component_wr_new();

LASTComponentAvailable last_components[] =
{
    {"title",        last_component_title_new},
    {"splits",       last_component_splits_new},
    {"timer",        last_component_timer_new},
    {"prev-segment", last_component_prev_segment_new},
    {"best-sum",     last_component_best_sum_new},
    {"pb",           last_component_pb_new},
    {"wr",           last_component_wr_new},
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
