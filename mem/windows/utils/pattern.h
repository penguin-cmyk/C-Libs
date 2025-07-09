//
// Created by sigma on 06/07/2025.
//

#ifndef LIBS_PATTERN_H
#define LIBS_PATTERN_H

#include <windows.h>
#include <tlhelp32.h>
#include <ctype.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "stdio.h"
#include "stdbool.h"
#include "ntdll.h"

static bool pattern_to_bytes(const char* pattern, uint8_t* out_bytes, char* out_mask, size_t max_len, size_t* out_len) {
    size_t i = 0;
    while (*pattern && i < max_len) {
        while (*pattern == ' ') pattern++;

        if (*pattern == '\0') break;

        if (*pattern == '?') {
            out_bytes[i] = 0x00;
            out_mask[i] = '?';
            if (*(pattern + 1) == '?') pattern++;
            pattern++;
        } else if (isxdigit(pattern[0]) && isxdigit(pattern[1])) {
            int byte;
            if (sscanf(pattern, "%2x", &byte) != 1) return false;
            out_bytes[i] = (uint8_t)byte;
            out_mask[i] = 'x';
            pattern += 2;
        } else {
            return false;
        }

        i++;
    }

    *out_len = i;
    return true;
}

static uintptr_t scan_region(HANDLE process, uintptr_t base, size_t size, const uint8_t* pattern, const char* mask, size_t pattern_len) {
    uint8_t* buffer = malloc(size);
    if (!buffer) return 0;

    SIZE_T bytes_read;
    if (!NtReadVirtualMemoryEx(process, (PVOID)base, buffer, size, &bytes_read) || bytes_read < pattern_len) {
        free(buffer);
        return 0;
    }

    for (size_t i = 0; i <= bytes_read - pattern_len; i++) {
        bool matched = true;
        for (size_t j = 0; j < pattern_len; j++) {
            if (mask[j] == 'x' && buffer[i + j] != pattern[j]) {
                matched = false;
                break;
            }
        }

        if (matched) {
            uintptr_t result = base + i;
            free(buffer);
            return result;
        }
    }

    free(buffer);
    return 0;
}

uintptr_t pattern_scan(Process* process, uintptr_t module_base, size_t module_size, const char* ida_pattern) {
    uint8_t pattern_bytes[256];
    char pattern_mask[256];
    size_t pattern_len;

    if (!pattern_to_bytes(ida_pattern, pattern_bytes, pattern_mask, sizeof(pattern_bytes), &pattern_len))
        return 0;

    uintptr_t current = module_base;
    uintptr_t end = module_base + module_size;
    MEMORY_BASIC_INFORMATION mbi;
    PSIZE_T outlen;
    while (current < end) {
        NTSTATUS status = ZwQueryVirtualMemory(
                process->handle,
                (PVOID)current,
                0,
                &mbi,
                sizeof(mbi),
                &outlen
        );

        if (status < 0)
            break;

        bool readable = mbi.State == MEM_COMMIT &&
                        !(mbi.Protect & PAGE_NOACCESS) &&
                        !(mbi.Protect & PAGE_GUARD);

        if (readable && (uintptr_t)mbi.BaseAddress >= module_base && (uintptr_t)mbi.BaseAddress < end) {
            uintptr_t region_start = (uintptr_t)mbi.BaseAddress;
            size_t region_size = mbi.RegionSize;

            if (region_start + region_size > end) {
                region_size = end - region_start;
            }

            uintptr_t found = scan_region(process->handle, region_start, region_size, pattern_bytes, pattern_mask, pattern_len);
            if (found != 0)
                return found; // to get the offset do ( found - process->baseAddress )
        }

        current = (uintptr_t)mbi.BaseAddress + mbi.RegionSize;
    }

    return 0;
}
#endif //LIBS_PATTERN_H
