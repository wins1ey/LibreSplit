#include "signature.h"
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/uio.h>
#include <unistd.h>

#include "memory.h"
#include "process.h"

#include <luajit.h>

extern game_process process;

// Error handling macro
#define HANDLE_ERROR(msg) \
    do {                  \
        perror(msg);      \
        return NULL;      \
    } while (0)

// Error logging function
void log_error(const char* format, ...)
{
    va_list args;
    va_start(args, format);
    fprintf(stderr, "Error in sig_scan: ");
    vfprintf(stderr, format, args);
    fprintf(stderr, "\n");
    va_end(args);
}

MemoryRegion* get_memory_regions(pid_t pid, int* count)
{
    char maps_path[256];
    if (snprintf(maps_path, sizeof(maps_path), "/proc/%d/maps", pid) < 0) {
        HANDLE_ERROR("Failed to create maps path");
    }

    FILE* maps_file = fopen(maps_path, "r");
    if (!maps_file) {
        HANDLE_ERROR("Failed to open maps file");
    }

    MemoryRegion* regions = NULL;
    int capacity = 0;
    *count = 0;

    char line[256];
    while (fgets(line, sizeof(line), maps_file)) {
        if (*count >= capacity) {
            capacity = capacity == 0 ? 10 : capacity * 2;
            MemoryRegion* temp = realloc(regions, capacity * sizeof(MemoryRegion));
            if (!temp) {
                free(regions);
                fclose(maps_file);
                HANDLE_ERROR("Failed to allocate memory for regions");
            }
            regions = temp;
        }

        uintptr_t start, end;
        if (sscanf(line, "%" SCNxPTR "-%" SCNxPTR, &start, &end) != 2) {
            continue; // Skip lines that don't match the expected format
        }
        regions[*count].start = start;
        regions[*count].end = end;
        (*count)++;
    }

    fclose(maps_file);
    return regions;
}

bool match_pattern(const uint8_t* data, const uint16_t* pattern, size_t pattern_size)
{
    for (size_t i = 0; i < pattern_size; ++i) {
        uint8_t byte = pattern[i] & 0xFF;
        bool ignore = (pattern[i] >> 8) & 0x1;
        if (!ignore && data[i] != byte) {
            return false;
        }
    }
    return true;
}

uint16_t* convert_signature(const char* signature, size_t* pattern_size)
{
    char* signature_copy = strdup(signature);
    if (!signature_copy) {
        return NULL;
    }

    char* token = strtok(signature_copy, " ");
    size_t size = 0;
    size_t capacity = 10;
    uint16_t* pattern = (uint16_t*)malloc(capacity * sizeof(uint16_t));
    if (!pattern) {
        free(signature_copy);
        return NULL;
    }

    while (token != NULL) {
        if (size >= capacity) {
            capacity *= 2;
            uint16_t* temp = (uint16_t*)realloc(pattern, capacity * sizeof(uint16_t));
            if (!temp) {
                free(pattern);
                free(signature_copy);
                return NULL;
            }
            pattern = temp;
        }

        if (strcmp(token, "??") == 0) {
            pattern[size] = 0xFF00; // Set the upper byte to 1 to indicate ignoring this byte
        } else {
            pattern[size] = strtol(token, NULL, 16);
        }
        size++;
        token = strtok(NULL, " ");
    }

    free(signature_copy);
    *pattern_size = size;
    return pattern;
}

bool validate_process_memory(pid_t pid, uintptr_t address, void* buffer, size_t size)
{
    struct iovec local_iov = { buffer, size };
    struct iovec remote_iov = { (void*)address, size };
    ssize_t nread = process_vm_readv(pid, &local_iov, 1, &remote_iov, 1, 0);

    return nread == (ssize_t)size;
}

int perform_sig_scan(lua_State* L)
{
    if (lua_gettop(L) != 2) {
        log_error("Invalid number of arguments: expected 2 (signature, offset)");
        lua_pushnil(L);
        return 1;
    }

    if (!lua_isstring(L, 1) || !lua_isnumber(L, 2)) {
        log_error("Invalid argument types: expected (string, number)");
        lua_pushnil(L);
        return 1;
    }

    pid_t p_pid = process.pid;
    const char* signature = lua_tostring(L, 1);
    int offset = lua_tointeger(L, 2);

    // Validate signature string
    if (strlen(signature) == 0) {
        log_error("Signature string cannot be empty");
        lua_pushnil(L);
        return 1;
    }

    size_t pattern_length;
    uint16_t* pattern = convert_signature(signature, &pattern_length);
    if (!pattern) {
        log_error("Failed to convert signature");
        lua_pushnil(L);
        return 1;
    }

    int regions_count = 0;
    MemoryRegion* regions = get_memory_regions(p_pid, &regions_count);
    if (!regions) {
        free(pattern);
        log_error("Failed to get memory regions");
        lua_pushnil(L);
        return 1;
    }

    for (int i = 0; i < regions_count; i++) {
        MemoryRegion region = regions[i];
        ssize_t region_size = region.end - region.start;
        uint8_t* buffer = malloc(region_size);
        if (!buffer) {
            free(pattern);
            free(regions);
            log_error("Failed to allocate memory for region buffer");
            lua_pushnil(L);
            return 1;
        }

        if (!validate_process_memory(p_pid, region.start, buffer, region_size)) {
            free(buffer);
            continue; // Continue to next region
        }

        for (size_t j = 0; j <= region_size - pattern_length; ++j) {
            if (match_pattern(buffer + j, pattern, pattern_length)) {
                uintptr_t result = region.start + j + offset;

                char full_hex_str[32]; // Increased buffer size for safety
                if (snprintf(full_hex_str, sizeof(full_hex_str), "0x%" PRIxPTR, result) < 0) {
                    free(buffer);
                    free(pattern);
                    free(regions);
                    log_error("Failed to convert result to string");
                    lua_pushnil(L);
                    return 1;
                }

                free(buffer);
                free(pattern);
                free(regions);

                lua_pushstring(L, full_hex_str);
                return 1;
            }
        }

        free(buffer);
    }

    free(pattern);
    free(regions);

    // No match found
    log_error("No match found for the given signature");
    lua_pushnil(L);
    return 1;
}
