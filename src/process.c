#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <lua.h>

#include "process.h"
#include "auto-splitter.h"

struct last_process process;

void execute_command(const char* command, char* buffer, char* output)
{
    FILE* pipe = popen(command, "r");
    if (!pipe)
    {
        fprintf(stderr, "Error executing command: %s\n", command);
        exit(1);
    }

    while (fgets(buffer, 128, pipe) != NULL)
    {
        strcat(output, buffer);
    }

    pclose(pipe);
}

/*
Gets the base address of a module
if `module` equals to a nullptr, the main process is used, else it will search for the base addr of the specified module
*/
uintptr_t find_base_address(const char* module)
{
    char maps_command[256];
    const char* module_to_grep = module == 0 ? process.name : module;

    snprintf(maps_command, sizeof(maps_command), "cat /proc/%d/maps | grep \"%.*s\" | head -n 1",
             process.pid, (int)strnlen(module_to_grep, 15), module_to_grep);

    char buffer[128];
    char maps_output[1024];
    maps_output[0] = '\0';

    execute_command(maps_command, buffer, maps_output);

    size_t dash_pos = strcspn(maps_output, "-");

    if (dash_pos != strlen(maps_output))
    {
        char first_number[32];
        strncpy(first_number, maps_output, dash_pos);
        first_number[dash_pos] = '\0';
        return strtoull(first_number, NULL, 16);
    }
    else
    {
        printf("Couldn't find base address\n");
        return 0;
    }
}

void stock_process_id(const char* processtarget)
{
    char pid_command[128];
    strcpy(pid_command, processtarget);

    char buffer[128];
    char pid_output[128];
    pid_output[0] = '\0';

    while (process.pid == 0 && atomic_load(&auto_splitter_enabled))
    {
        execute_command(pid_command, buffer, pid_output);
        size_t space_pos = strcspn(pid_output, " ");
        if (space_pos != strlen(pid_output))
        {
            printf("Multiple PID's found for process: %s\n", process.name);
        }
        process.pid = strtoul(pid_output, NULL, 10);
        printf("\033[2J\033[1;1H"); // Clear the console
        usleep(100000);
        printf("%s isn't running.\n", process.name);
    }
    
    if (process.pid != 0)
    {
        printf("\033[2J\033[1;1H"); // Clear the console
        printf("Process: %s\n", process.name);
        printf("PID: %u\n", process.pid);
        process.base_address = find_base_address(0);
        process.dll_address = process.base_address;
    }
}

int find_process_id(lua_State* L)
{
    process.name = lua_tostring(L, 1);
    char command[256];
    printf("\033[2J\033[1;1H"); // Clear the console
    snprintf(command, sizeof(command), "pgrep \"%.*s\"", (int)strnlen(process.name, 15), process.name);

    stock_process_id(command);

    return 0;
}

int process_exists()
{
    int result = kill(process.pid, 0);
    return result == 0;
}