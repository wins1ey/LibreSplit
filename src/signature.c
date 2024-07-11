#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/uio.h>
#include <stdint.h>
#include <stdbool.h>

// Include your headers
#include "signature.h"
#include "memory.h"
#include "process.h"

#include <luajit.h>

extern game_process process;

MemoryRegion* get_memory_regions(pid_t pid, int* count)
{
    char maps_path[256];
    sprintf(maps_path, "/proc/%d/maps", pid);
    FILE* maps_file = fopen(maps_path, "r");
    if (!maps_file) {
        perror("Failed to open maps file");
        return NULL;
    }

    MemoryRegion* regions = NULL;
    int capacity = 0;
    *count = 0;

    char line[256];
    while (fgets(line, sizeof(line), maps_file))
    {
        if (*count >= capacity)
        {
            capacity = capacity == 0 ? 10 : capacity * 2;
            regions = (MemoryRegion*)realloc(regions, capacity * sizeof(MemoryRegion));
            if (!regions) {
                perror("Failed to allocate memory for regions");
                fclose(maps_file);
                return NULL;
            }
        }

        uintptr_t start, end;
        if (sscanf(line, "%lx-%lx", &start, &end) != 2) {
            continue; // Skip lines that don't match the expected format
        }
        regions[*count].start = start;
        regions[*count].end = end;
        (*count)++;
    }

    fclose(maps_file);
    return regions;
}

bool match_pattern(const uint8_t* data, const int* pattern, size_t pattern_size)
{
    for (size_t i = 0; i < pattern_size; ++i)
    {
        if (pattern[i] != -1 && data[i] != pattern[i])
            return false;
    }
    return true;
}

int* convert_signature(const char* signature, size_t* pattern_size)
{
    char* signature_copy = strdup(signature);
    if (!signature_copy) {
        return NULL;
    }

    char* token = strtok(signature_copy, " ");
    size_t size = 0;
    size_t capacity = 10;
    int* pattern = (int*)malloc(capacity * sizeof(int));
    if (!pattern) {
        free(signature_copy);
        return NULL;
    }

    while (token != NULL)
    {
        if (size >= capacity)
        {
            capacity *= 2;
            int* temp = (int*)realloc(pattern, capacity * sizeof(int));
            if (!temp) {
                free(pattern);
                free(signature_copy);
                return NULL;
            }
            pattern = temp;
        }

        if (strcmp(token, "??") == 0)
            pattern[size] = -1;
        else
            pattern[size] = strtol(token, NULL, 16);
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

    return nread == size;
}

int find_signature(lua_State* L)
{
    pid_t pid = process.pid;
    const char* signature = lua_tostring(L, 1);
    size_t pattern_length;

    int* pattern = convert_signature(signature, &pattern_length);
    if (!pattern) {
        printf("Failed to convert signature\n");
        return -1; // Return an error code
    }

    int regions_count = 0;
    MemoryRegion* regions = get_memory_regions(process.pid, &regions_count);
    if (!regions) {
        printf("Failed to get memory regions\n");
        free(pattern);
        return -1; // Return an error code
    }

    for (int i = 0; i < regions_count; i++)
    {
        MemoryRegion region = regions[i];
        ssize_t region_size = region.end - region.start;
        uint8_t* buffer = (uint8_t*)malloc(region_size);
        if (!buffer)
        {
            printf("Failed to allocate memory for buffer\n");
            free(pattern);
            free(regions);
            return -1; // Return an error code
        }

        if (!validate_process_memory(pid, region.start, buffer, region_size))
        {
            printf("Failed to read memory\n");
            free(pattern);
            free(regions);
            free(buffer);
            return -1; // Return an error code
        }
        else
        {
            for (size_t i = 0; i <= region_size - pattern_length; ++i)
            {
                if (match_pattern(buffer + i, pattern, pattern_length))
                {
                    uintptr_t result = region.start + i;
                    free(pattern);
                    free(regions);
                    free(buffer);
                    return result;
                }
            }
        }

        free(buffer);
    }

    free(pattern);
    free(regions);
    return -1; // Return an error code if no match is found
}