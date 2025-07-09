//
// Created by sigma on 06/07/2025.
//
#include "stdio.h"
#include "stdbool.h"
#include "mem.h"

#include "utils/pattern.h"

// Helper functions
DWORD get_pid(const char* process) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    if (!Process32First(snapshot, &entry)) {
        CloseHandle(snapshot);
        return 0;
    }

    do {
        if (strcmp(entry.szExeFile, process) == 0) {
            CloseHandle(snapshot);
            return entry.th32ProcessID;
        }
    } while (Process32Next(snapshot, &entry));

    CloseHandle(snapshot);
    return 0;
}

static inline HANDLE open_handle(DWORD pid) {
    HANDLE process_handle = NULL;

    CLIENT_ID client_id = { 0 };
    client_id.UniqueProcess = (HANDLE)(uintptr_t)pid;
    client_id.UniqueThread = NULL;

    OBJECT_ATTRIBUTES obj_attrs = { 0 };
    obj_attrs.Length = sizeof(OBJECT_ATTRIBUTES);
    obj_attrs.RootDirectory = NULL;
    obj_attrs.ObjectName = NULL;
    obj_attrs.Attributes = 0;
    obj_attrs.SecurityDescriptor = NULL;
    obj_attrs.SecurityQualityOfService = NULL;

    NtOpenProcess(&process_handle, PROCESS_ALL_ACCESS, &obj_attrs, &client_id);
    return process_handle;
}


uintptr_t base_address(DWORD pid) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    MODULEENTRY32 entry;
    entry.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(snapshot, &entry) == 0) {
        CloseHandle(snapshot);
        return 0;
    }

    CloseHandle(snapshot);

    return (uintptr_t)entry.modBaseAddr;
}

uintptr_t base_size(DWORD pid) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    MODULEENTRY32 entry;
    entry.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(snapshot, &entry) == 0) {
        CloseHandle(snapshot);
        return 0;
    }

    CloseHandle(snapshot);

    return (uintptr_t)entry.modBaseSize;
}

static inline bool read_memory(Process* proc, uintptr_t address, void* out_buffer, size_t size) {
    PSIZE_T bytes_read;
    return NtReadVirtualMemoryEx(proc->handle, (PVOID)address, out_buffer, size, (PSIZE_T) &bytes_read);
}

static inline bool write_memory(Process* proc, uintptr_t address, void* data, size_t size) {
    SIZE_T bytes_written;
    return NtWriteVirtualMemory( proc->handle, (PVOID)address, data, size, &bytes_written);
}

static inline uintptr_t rebase(Process* proc, uintptr_t address) {
    return proc->baseAddress + address;
}

static inline void* allocate_memory(Process* proc, SIZE_T size) {
    PVOID base = NULL;
    SIZE_T region_size = size;

    NtAllocateVirtualMemoryEx(proc->handle, &base, &region_size, MEM_COMMIT | MEM_RESERVE, PAGE_EXECUTE_READWRITE, NULL, 0);
    return base;
}

static inline void terminate(Process* proc) {
    TerminateProcess(proc->handle, 1);
    NtClose(proc->handle);
}

static inline uintptr_t find_pattern(Process* process, const char* pattern) {
    return pattern_scan(process, process->baseAddress, process->baseSize, pattern);
}

Module* get_module(Process* process, const char* name) {
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, process->pid);
    if (snapshot == INVALID_HANDLE_VALUE) {
       return NULL;
    }

    MODULEENTRY32 entry;
    entry.dwSize = sizeof(MODULEENTRY32);

    if (Module32First(snapshot, &entry) == 0) {
        CloseHandle(snapshot);
        return NULL;
    }

    do {
        if (strcmp(entry.szModule, name ) == 0) {
            Module* mod = malloc(sizeof(Module));
            if (!mod) return NULL;

            mod->entry = entry;
            mod->baseAddress = (uintptr_t)entry.hModule;
            return mod;
        }
    } while (Module32Next(snapshot, &entry ) != 0);

    return NULL;
}

// Main
Process* new_process(const char* name) {
    DWORD pid = get_pid(name);
    if (pid == 0) goto exit;

    HANDLE handle = open_handle(pid);
    if (handle == NULL) goto exit;

    uintptr_t base = base_address(pid);
    if (base == 0) goto exit;

    uintptr_t size = base_size(pid);
    if (size == 0) goto exit;

    // ------------------------------------- \\

    Process* new = (Process*)malloc(sizeof(Process));
    new->baseAddress = base;
    new->baseSize = size;
    new->handle = handle;
    new->pid = pid;

    #ifdef FUNCS
        new->ReadMemory = read_memory;
        new->WriteMemory = write_memory;
        new->Rebase = rebase;
        new->Terminate = terminate;
        new->PatternScan = find_pattern;
        new->Module = get_module;
        new->Allocate = allocate_memory;
    #endif

    return new;

    exit:
        printf("Process %s not found", name);
        exit(0);
}

