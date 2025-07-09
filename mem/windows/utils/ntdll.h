//
// Created by sigma on 07/07/2025.
//

#ifndef LIBS_NTDLL_H
#define LIBS_NTDLL_H
#include <windows.h>

// types
typedef _Return_type_success_(return >= 0) long NTSTATUS;
typedef struct _UNICODE_STRING
{
    USHORT Length;
    USHORT MaximumLength;
    _Field_size_bytes_part_opt_(MaximumLength, Length) PWCH Buffer;
} UNICODE_STRING, *PUNICODE_STRING;
typedef const UNICODE_STRING *PCUNICODE_STRING;

typedef struct _OBJECT_ATTRIBUTES
{
    ULONG Length;
    HANDLE RootDirectory;
    PCUNICODE_STRING ObjectName;
    ULONG Attributes;
    PVOID SecurityDescriptor;
    PVOID SecurityQualityOfService;
} OBJECT_ATTRIBUTES, *POBJECT_ATTRIBUTES;
typedef const OBJECT_ATTRIBUTES *PCOBJECT_ATTRIBUTES;

typedef struct _CLIENT_ID
{
    HANDLE UniqueProcess;
    HANDLE UniqueThread;
} CLIENT_ID, *PCLIENT_ID;

extern NTSTATUS NtReadVirtualMemoryEx(HANDLE ProcessHandle,PVOID BaseAddress,PVOID Buffer,ULONG Size,PSIZE_T NumberOfBytesRead);
extern NTSTATUS NtWriteVirtualMemory(HANDLE ProcessHandle,PVOID BaseAddress,PVOID Buffer,SIZE_T NumberOfBytesToWrite,PSIZE_T NumberOfBytesRead);
extern NTSTATUS NtOpenProcess(PHANDLE ProcessHandle, ACCESS_MASK DesiredAccess, PCOBJECT_ATTRIBUTES ObjectAttributes, PCLIENT_ID client);
extern NTSTATUS NtAllocateVirtualMemoryEx(HANDLE ProcessHandle, PVOID* BaseAddress, PSIZE_T RegionSize, ULONG AllocationType, ULONG PageProtection, PMEM_EXTENDED_PARAMETER ExtendedParameters, ULONG ExtendedParameterCount);
extern NTSTATUS ZwQueryVirtualMemory(HANDLE ProcessHandle,PVOID BaseAddress,int MemoryInformationClass,PVOID MemoryInformation,SIZE_T MemoryInformationLength,PSIZE_T ReturnLength);
extern NTSTATUS NtClose(HANDLE ProcessHandle);

#endif //LIBS_NTDLL_H
