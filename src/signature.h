#ifndef __SIGNATURE_H__
#define __SIGNATURE_H__

#include <luajit.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <sys/types.h>

typedef struct {
    uintptr_t start;
    uintptr_t end;
} MemoryRegion;

MemoryRegion* get_memory_regions(pid_t pid, int* count);
bool match_pattern(const uint8_t* data, const uint16_t* pattern, size_t pattern_size);
uint16_t* convert_signature(const char* signature, size_t* pattern_size);
bool validate_process_memory(pid_t pid, uintptr_t address, void* buffer, size_t size);
int find_signature(lua_State* L);

#endif /* __SIGNATURE_H__ */