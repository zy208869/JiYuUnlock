#include "process.h"
#include <iostream>
#include <tlhelp32.h>

DWORD GetProcessIdByName(const std::string& processName) {
    DWORD processId = 0;
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(snapshot, &processEntry)) {
        do {
            if (_stricmp(processEntry.szExeFile, processName.c_str()) == 0) {
                processId = processEntry.th32ProcessID;
                break;
            }
        } while (Process32Next(snapshot, &processEntry));
    }

    CloseHandle(snapshot);
    return processId;
}

bool KillProcessByName(const std::string& processName) {
    DWORD pid = GetProcessIdByName(processName);
    if (pid == 0) {
        std::cerr << "[!] 未找到进程: " << processName << std::endl;
        return false;
    }

    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == NULL) {
        std::cerr << "[!] 无法打开进程，错误码: " << GetLastError() << std::endl;
        return false;
    }

    bool result = TerminateProcess(hProcess, 0) != 0;
    CloseHandle(hProcess);
    return result;
}

bool SuspendProcessByName(const std::string& processName) {
    DWORD pid = GetProcessIdByName(processName);
    if (pid == 0) {
        std::cerr << "[!] 未找到进程: " << processName << std::endl;
        return false;
    }

    HANDLE hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, pid);
    if (hProcess == NULL) {
        std::cerr << "[!] 无法打开进程，错误码: " << GetLastError() << std::endl;
        return false;
    }

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        CloseHandle(hProcess);
        return false;
    }

    THREADENTRY32 threadEntry;
    threadEntry.dwSize = sizeof(THREADENTRY32);
    bool success = true;

    if (Thread32First(snapshot, &threadEntry)) {
        do {
            if (threadEntry.th32OwnerProcessID == pid) {
                HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, threadEntry.th32ThreadID);
                if (hThread) {
                    if (SuspendThread(hThread) == (DWORD)-1) {
                        success = false;
                    }
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(snapshot, &threadEntry));
    }

    CloseHandle(snapshot);
    CloseHandle(hProcess);
    return success;
}

bool ResumeProcessByName(const std::string& processName) {
    DWORD pid = GetProcessIdByName(processName);
    if (pid == 0) {
        std::cerr << "[!] 未找到进程: " << processName << std::endl;
        return false;
    }

    HANDLE hProcess = OpenProcess(PROCESS_SUSPEND_RESUME, FALSE, pid);
    if (hProcess == NULL) {
        std::cerr << "[!] 无法打开进程，错误码: " << GetLastError() << std::endl;
        return false;
    }

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        CloseHandle(hProcess);
        return false;
    }

    THREADENTRY32 threadEntry;
    threadEntry.dwSize = sizeof(THREADENTRY32);
    bool success = true;

    if (Thread32First(snapshot, &threadEntry)) {
        do {
            if (threadEntry.th32OwnerProcessID == pid) {
                HANDLE hThread = OpenThread(THREAD_SUSPEND_RESUME, FALSE, threadEntry.th32ThreadID);
                if (hThread) {
                    if (ResumeThread(hThread) == (DWORD)-1) {
                        success = false;
                    }
                    CloseHandle(hThread);
                }
            }
        } while (Thread32Next(snapshot, &threadEntry));
    }

    CloseHandle(snapshot);
    CloseHandle(hProcess);
    return success;
}