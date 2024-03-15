#include <linux/limits.h>
#include <stdio.h>
#include <pwd.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>

#include <jansson.h>

#include "settings.h"

void get_LAST_folder_path(char* out_path) {
    struct passwd * pw = getpwuid(getuid());
    char* XDG_CONFIG_HOME = getenv("XDG_CONFIG_HOME");
    char* base_dir = strcat(pw->pw_dir, "/.config/LAST");
    if (XDG_CONFIG_HOME != NULL) {
        char config_dir[PATH_MAX] = {0};
        strcpy(config_dir, XDG_CONFIG_HOME);
        strcat(config_dir, "/LAST");
        strcpy(base_dir, config_dir);
    }
    strcpy(out_path, base_dir);
}

void last_update_setting(const char *setting, json_t *value)
{
    char settings_path[PATH_MAX];
    get_LAST_folder_path(settings_path);
    strcat(settings_path, "/settings.json");

    // Load existing settings
    json_t *root = NULL;
    FILE *file = fopen(settings_path, "r");
    if (file)
    {
        json_error_t error;
        root = json_loadf(file, 0, &error);
        fclose(file);
        if (!root)
        {
            printf("Failed to load settings: %s\n", error.text);
            return;
        }
    }
    else
    {
        // If file doesn't exist, create a new settings object
        root = json_object();
    }

    // Update specific setting
    json_t *last_obj = json_object_get(root, "LAST");
    if (!last_obj)
    {
        last_obj = json_object();
        json_object_set(root, "LAST", last_obj);
    }
    json_object_set_new(last_obj, setting, value);

    // Save updated settings back to the file
    FILE *output_file = fopen(settings_path, "w");
    if (output_file)
    {
        json_dumpf(root, output_file, JSON_INDENT(4));
        fclose(output_file);
    }
    else
    {
        printf("Failed to save settings to %s\n", settings_path);
    }

    json_decref(root);
}

json_t *load_settings()
{
    char settings_path[PATH_MAX];
    get_LAST_folder_path(settings_path);
    strcat(settings_path, "/settings.json");

    FILE *file = fopen(settings_path, "r");
    if (file)
    {
        json_error_t error;
        json_t *root = json_loadf(file, 0, &error);
        fclose(file);
        if (!root)
        {
            printf("Failed to load settings: %s\n", error.text);
            return NULL;
        }
        return root;
    }
    else
    {
        printf("Failed to open settings file\n");
        return NULL;
    }
}

json_t *get_setting_value(const char *section, const char *setting)
{
    json_t *root = load_settings();
    if (!root)
    {
        return NULL;
    }

    json_t *section_obj = json_object_get(root, section);
    if (!section_obj)
    {
        printf("Section '%s' not found\n", section);
        json_decref(root);
        return NULL;
    }

    json_t *value = json_object_get(section_obj, setting);
    if (!value)
    {
        printf("Setting '%s' not found in section '%s'\n", setting, section);
        json_decref(root);
        return NULL;
    }

    // Increment the reference count before returning
    json_incref(value);

    // Release the root object
    json_decref(root);

    return value;
}