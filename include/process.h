#pragma once
#include <string>
#include <windows.h>

DWORD GetProcessIdByName(const std::string& processName);
bool KillProcessByName(const std::string& processName);
bool SuspendProcessByName(const std::string& processName);
bool ResumeProcessByName(const std::string& processName);