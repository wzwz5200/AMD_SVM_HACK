#include "tool.h"

ULONG64 GetModule(UINT32 pid, const wchar_t* moduleName)
{
    HANDLE targetProcessHandle = OpenProcess(PROCESS_QUERY_INFORMATION, 0, pid);
    if (!targetProcessHandle || targetProcessHandle == INVALID_HANDLE_VALUE)
        return 0;

    ULONG64 currentAddress = 0;
    MEMORY_BASIC_INFORMATION memoryInformation;
    while (VirtualQueryEx(targetProcessHandle, reinterpret_cast<void*>(currentAddress), &memoryInformation, sizeof(MEMORY_BASIC_INFORMATION64)))
    {
        if (memoryInformation.Type == MEM_MAPPED || memoryInformation.Type == MEM_IMAGE)
        {
            constexpr size_t bufferSize = 1024;
            void* buffer = malloc(bufferSize);

            size_t bytesOut;
            NTSTATUS status = NtQueryVirtualMemory(targetProcessHandle, memoryInformation.BaseAddress, MemoryMappedFilenameInformation, buffer, bufferSize, &bytesOut);
            if (status == 0)
            {
                UNICODE_STRING* stringBuffer = static_cast<UNICODE_STRING*>(buffer);
                if (wcsstr(stringBuffer->Buffer, moduleName) && !wcsstr(stringBuffer->Buffer, L".mui"))
                {
                    free(buffer);
                    CloseHandle(targetProcessHandle);
                    return reinterpret_cast<ULONG64>(memoryInformation.BaseAddress);
                }
            }

            free(buffer);
        }

        currentAddress = reinterpret_cast<ULONG64>(memoryInformation.BaseAddress) + memoryInformation.RegionSize;
    }

    CloseHandle(targetProcessHandle);
    return 0;
}

UINT32 LookupProcessId(const wchar_t* processName)
{
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot)
    {
        PROCESSENTRY32 entry = { 0 };
        entry.dwSize = sizeof(entry);
        if (Process32First(snapshot, &entry))
        {
            do
            {
                if (0 == _wcsicmp(entry.szExeFile, processName))
                {
                    CloseHandle(snapshot);
                    return entry.th32ProcessID;
                }
            } while (Process32Next(snapshot, &entry));
        }

        CloseHandle(snapshot);
    }

    return 0;
}