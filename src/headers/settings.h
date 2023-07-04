#ifndef __settings_h__
#define __settings_h__

#include <jansson.h>

void last_update_setting(const char *setting, json_t *value);
json_t *get_setting_value(const char *section, const char *setting);

#endif