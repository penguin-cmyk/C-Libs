//
// Created by sigma on 06/07/2025.
//

#ifndef LIBS_MEM_H
#define LIBS_MEM_H

#include <windows.h>
#include <tlhelp32.h>
#include "stdbool.h"
#include "utils/ntdll.h"

#define FUNCS // remove if you don't want the functions in the PROCESS directly and do it manually

typedef struct Process Process;

typedef struct {
    MODULEENTRY32 entry;
    uintptr_t baseAddress;
} Module;

struct Process {
    HANDLE handle;
    DWORD pid;
    uintptr_t baseAddress;
    uintptr_t baseSize;

    #ifdef FUNCS
        bool (*ReadMemory)(Process* process, uintptr_t address, void* out_buffer, size_t size);
        bool (*WriteMemory)(Process* process, uintptr_t address, void* data, size_t size);
        uintptr_t (*Rebase)(Process* process, uintptr_t address);
        uintptr_t (*PatternScan)(Process* proc, const char* pattern);
        Module* (*Module)(Process* process, const char* name);
        void* (*Allocate)(Process* process, SIZE_T size);
        void (*Terminate)(Process* process);
    #endif
};

Process* new_process(const char* name);

#ifndef FUNCS
    uintptr_t rebase(Process* process, uintptr_t address);
    bool write_memory(Process* process, uintptr_t address, const void* data, size_t size);
    bool read_memory(Process* process, uintptr_t address, void* out_buffer, size_t size);
    void terminate(Process* process)
    uintptr_t find_pattern(Process* process, const char* pattern);
    Module* get_module(Process* process, const char* name);
    void* allocate_memory(Process* process, SIZE_T size);
#endif


#endif //LIBS_MEM_H
