#pragma once
#include <Windows.h>
#include <winternl.h>
#include <TlHelp32.h>

typedef enum _MEMORY_INFORMATION_CLASS
{
    MemoryBasicInformation, // MEMORY_BASIC_INFORMATION
    MemoryWorkingSetInformation, // MEMORY_WORKING_SET_INFORMATION
    MemoryMappedFilenameInformation, // UNICODE_STRING
    MemoryRegionInformation, // MEMORY_REGION_INFORMATION
    MemoryWorkingSetExInformation, // MEMORY_WORKING_SET_EX_INFORMATION
    MemorySharedCommitInformation, // MEMORY_SHARED_COMMIT_INFORMATION
    MemoryImageInformation,
    MemoryRegionInformationEx,
    MemoryPrivilegedBasicInformation,
    MemoryEnclaveImageInformation,
    MemoryBasicInformationCapped
} MEMORY_INFORMATION_CLASS;

EXTERN_C NTSYSAPI NTSTATUS NTAPI NtQueryVirtualMemory(HANDLE processHandle, void* baseAddress, MEMORY_INFORMATION_CLASS memoryInformationClass, void* memoryInformation, size_t memoryInformationLength, size_t* returnLength);

ULONG64 GetModule(UINT32 pid, const wchar_t* moduleName);

UINT32 LookupProcessId(const wchar_t* processName);

