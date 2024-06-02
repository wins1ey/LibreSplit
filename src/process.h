#ifndef __PROCESS_H__
#define __PROCESS_H__

#include <linux/limits.h>
#include <stdbool.h>
#include <stdint.h>

#include <luajit.h>

struct last_process
{
    const char *name;
    int pid;
    uintptr_t base_address;
    uintptr_t dll_address;
};
typedef struct last_process last_process;

typedef struct ProcessMap {
    uint64_t start;
    char name[PATH_MAX];
} ProcessMap;
extern uint32_t p_maps_cache_size;

uintptr_t find_base_address(const char* module);
int process_exists();
int find_process_id(lua_State* L);
int getPid(lua_State* L);
bool parseMapsLine(char* line, ProcessMap *map);

#endif /* __PROCESS_H__ */
