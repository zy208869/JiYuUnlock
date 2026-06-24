
#include <iostream>
#include <windows.h>
#include <fstream>
#include <vector>
#include <string>
#include <shlwapi.h>
#include <sys/stat.h>
#include "network.h"
#include "process.h"
#include "utils.h"

#pragma comment(lib, "shlwapi.lib")

std::vector<std::string> g_text;

// 获取 exe 所在目录（带反斜杠）
std::string GetExeDirectory() {
    char buffer[MAX_PATH];
    GetModuleFileNameA(NULL, buffer, MAX_PATH);
    PathRemoveFileSpecA(buffer);
    std::string dir = buffer;
    if (!dir.empty() && dir.back() != '\\') dir += '\\';
    return dir;
}

// 创建目录（如果不存在）
bool CreateDirectoryIfNotExists(const std::string& path) {
    if (CreateDirectoryA(path.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS)
        return true;
    return false;
}

// 写入语言文件
bool WriteLanguageFile(const std::string& fullPath, const std::vector<std::string>& lines) {
    std::ofstream file(fullPath, std::ios::binary);
    if (!file.is_open()) return false;
    for (const auto& line : lines) {
        file << line << "\n";
    }
    file.close();
    return true;
}

// 加载语言文件
bool LoadLanguage(const std::string& filename) {
    std::string exeDir = GetExeDirectory();
    std::string fullPath = exeDir + filename;
    std::ifstream file(fullPath, std::ios::binary);
    if (!file.is_open()) return false;

    g_text.clear();
    std::string line;
    while (std::getline(file, line)) {
        if (!line.empty() && line.back() == '\r') line.pop_back();
        g_text.push_back(line);
    }
    file.close();
    return !g_text.empty();
}

// 获取字符串
const std::string& getStr(size_t idx) {
    static std::string empty;
    if (idx < g_text.size()) return g_text[idx];
    return empty;
}

void printBanner() {
    std::cout << getStr(0) << std::endl;
    std::cout << getStr(1) << std::endl;
    std::cout << getStr(2) << std::endl;
    std::cout << getStr(3) << std::endl;
}

// ---------- 默认语言内容（用于释放文件） ----------
const std::vector<std::string> default_zh = {
    "========================================",
    "  极域电子教室 v4.0 2015 反控程序",
    "  仅供学习研究使用，请勿用于非法用途！",
    "========================================",
    "[!] 警告: 无法获取调试权限，部分功能可能受限。",
    "[!] 请尝试以管理员身份运行此程序。",
    "[!] 网络初始化失败，模拟教师端功能将不可用。",
    "",
    "请选择操作:",
    "1. 终止学生端进程 (StudentMain.exe)",
    "2. 挂起学生端进程 (StudentMain.exe)",
    "3. 恢复挂起的学生端进程",
    "4. 模拟教师端指令 (发送 '取消黑屏' 广播)",
    "5. 退出",
    "请输入数字: ",
    "[+] 成功终止 StudentMain.exe",
    "[!] 终止进程失败，可能需要管理员权限或进程受到保护。",
    "[+] 成功挂起 StudentMain.exe",
    "[!] 挂起进程失败。",
    "[+] 成功恢复 StudentMain.exe",
    "[!] 恢复进程失败。",
    "[+] 已发送 '取消黑屏' 广播指令。",
    "[!] 发送广播指令失败。",
    "退出程序。",
    "无效输入，请重新选择。"
};

const std::vector<std::string> default_en = {
    "========================================",
    "  JiYu Classroom v4.0 2015 Unlock Tool",
    "  For educational research only!",
    "========================================",
    "[!] Warning: Cannot get debug privilege. Some features may be limited.",
    "[!] Please run as Administrator.",
    "[!] Network initialization failed. Teacher emulation unavailable.",
    "",
    "Select an action:",
    "1. Terminate student process (StudentMain.exe)",
    "2. Suspend student process (StudentMain.exe)",
    "3. Resume student process (StudentMain.exe)",
    "4. Simulate teacher command (send 'unblack' broadcast)",
    "5. Exit",
    "Enter number: ",
    "[+] Successfully terminated StudentMain.exe",
    "[!] Failed to terminate process. Need admin rights or process protected.",
    "[+] Successfully suspended StudentMain.exe",
    "[!] Failed to suspend process.",
    "[+] Successfully resumed StudentMain.exe",
    "[!] Failed to resume process.",
    "[+] 'Unblack' broadcast command sent.",
    "[!] Failed to send broadcast.",
    "Exiting program.",
    "Invalid input, please try again."
};

int main() {
    // ----- 确保 lang 目录存在 -----
    std::string exeDir = GetExeDirectory();
    std::string langDir = exeDir + "lang\\";
    CreateDirectoryIfNotExists(langDir);

    // ----- 检查并释放语言文件 -----
    std::string zhPath = langDir + "zh_cn.lang";
    std::string enPath = langDir + "en_us.lang";

    std::ifstream testZh(zhPath);
    if (!testZh.is_open()) {
        WriteLanguageFile(zhPath, default_zh);
    }
    testZh.close();

    std::ifstream testEn(enPath);
    if (!testEn.is_open()) {
        WriteLanguageFile(enPath, default_en);
    }
    testEn.close();

    // ----- 语言选择 -----
    int choice = 0;
    std::cout << "Select language (1. Chinese  2. English): ";
    std::cin >> choice;

    bool loaded = false;
    if (choice == 1) {
        loaded = LoadLanguage("lang\\zh_cn.lang");
        if (loaded) SetConsoleOutputCP(CP_UTF8);
    } else if (choice == 2) {
        loaded = LoadLanguage("lang\\en_us.lang");
    } else {
        std::cout << "Invalid, default to English." << std::endl;
        loaded = LoadLanguage("lang\\en_us.lang");
    }

    if (!loaded) {
        std::cerr << "Warning: language file load failed, using built-in English." << std::endl;
        g_text = default_en;
    }

    // ----- 提权 -----
    if (!EnableDebugPrivilege()) {
        std::cerr << getStr(4) << std::endl;
        std::cerr << getStr(5) << std::endl;
    }

    printBanner();

    // ----- 网络初始化 -----
    NetworkManager netManager;
    if (!netManager.Init()) {
        std::cerr << getStr(6) << std::endl;
    }

    int op;
    while (true) {
        std::cout << getStr(7) << std::endl;
        std::cout << getStr(8) << std::endl;
        std::cout << getStr(9) << std::endl;
        std::cout << getStr(10) << std::endl;
        std::cout << getStr(11) << std::endl;
        std::cout << getStr(12) << std::endl;
        std::cout << getStr(13);
        std::cin >> op;

        switch (op) {
            case 1:
                if (KillProcessByName("StudentMain.exe"))
                    std::cout << getStr(14) << std::endl;
                else
                    std::cerr << getStr(15) << std::endl;
                break;
            case 2:
                if (SuspendProcessByName("StudentMain.exe"))
                    std::cout << getStr(16) << std::endl;
                else
                    std::cerr << getStr(17) << std::endl;
                break;
            case 3:
                if (ResumeProcessByName("StudentMain.exe"))
                    std::cout << getStr(18) << std::endl;
                else
                    std::cerr << getStr(19) << std::endl;
                break;
            case 4:
                if (netManager.SendCancelBlackScreen())
                    std::cout << getStr(20) << std::endl;
                else
                    std::cerr << getStr(21) << std::endl;
                break;
            case 5:
                std::cout << getStr(22) << std::endl;
                WSACleanup();
                return 0;
            default:
                std::cout << getStr(23) << std::endl;
        }
    }

    return 0;
}