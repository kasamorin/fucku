#include "openUrl.h"
#include <cstdlib>
#include <cstdio>      // for popen/pclose
#include <string>
#include <fstream>
#include <memory>       // for std::unique_ptr
#include <regex>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

// 辅助：执行 shell 命令并返回输出（去除末尾换行）
std::string execShell(const std::string& cmd) {
    std::unique_ptr<FILE, int(*)(FILE*)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe) return "";
    std::string result;
    char buffer[128];
    while (fgets(buffer, sizeof(buffer), pipe.get()) != nullptr) {
        result += buffer;
    }
    // 去除末尾换行
    if (!result.empty() && result.back() == '\n') result.pop_back();
    return result;
}

// 获取默认浏览器的窗口类名（Linux）
std::string getDefaultBrowserClass() {
    // 1. 查询默认的 https 处理程序
    std::string desktop = execShell("xdg-mime query default x-scheme-handler/https 2>/dev/null");
    if (desktop.empty()) {
        // 也可能用 http 作为备选
        desktop = execShell("xdg-mime query default x-scheme-handler/http 2>/dev/null");
    }
    if (desktop.empty()) return "";

    // 2. 查找 .desktop 文件路径
    // 常见位置：/usr/share/applications/ 和 ~/.local/share/applications/
    std::string desktopPath;
    const char* home = getenv("HOME");
    std::vector<std::string> searchPaths = {
        "/usr/share/applications/",
        "/usr/local/share/applications/",
        std::string(home) + "/.local/share/applications/"
    };
    for (const auto& dir : searchPaths) {
        std::string full = dir + desktop;
        std::ifstream f(full.c_str());
        if (f.good()) {
            desktopPath = full;
            break;
        }
    }
    if (desktopPath.empty()) return "";

    // 3. 解析 .desktop 文件，查找 StartupWMClass
    std::ifstream file(desktopPath);
    std::string line;
    std::regex wmclass_regex(R"(^StartupWMClass=(.+)$)");
    std::regex exec_regex(R"(^Exec=(.+)$)");
    std::string wmclass, execLine;

    while (std::getline(file, line)) {
        std::smatch match;
        if (std::regex_search(line, match, wmclass_regex)) {
            wmclass = match[1];
            break;
        }
        if (std::regex_search(line, match, exec_regex)) {
            execLine = match[1];
        }
    }

    if (!wmclass.empty()) return wmclass;

    // 4. 如果没有 StartupWMClass，从 Exec 提取可执行文件名（不含参数）
    if (!execLine.empty()) {
        // 去除可能的前缀（如 env 等）
        size_t firstSpace = execLine.find(' ');
        std::string exe = (firstSpace == std::string::npos) ? execLine : execLine.substr(0, firstSpace);
        // 去除路径，只保留基名
        size_t lastSlash = exe.find_last_of('/');
        if (lastSlash != std::string::npos) exe = exe.substr(lastSlash + 1);
        // 去除可能的 % 等
        return exe;
    }

    return "";
}

#ifdef __linux__
void bringBrowserToFront() {
    // 检查 xdotool 是否存在
    if (std::system("command -v xdotool > /dev/null 2>&1") != 0) return;

    // 获取默认浏览器窗口类名
    std::string browserClass = getDefaultBrowserClass();
    if (browserClass.empty()) {
        // 回退到硬编码列表
        const char* fallbackClasses[] = {
            "firefox", "Firefox",
            "chromium", "Chromium",
            "google-chrome", "Google-chrome",
            "brave-browser", "Brave-browser"
        };
        for (const char* cls : fallbackClasses) {
            std::string cmd = "xdotool search --class \"" + std::string(cls) + 
                              "\" windowactivate 2>/dev/null";
            if (std::system(cmd.c_str()) == 0) break;
        }
    } else {
        // 使用检测到的类名
        std::string cmd = "xdotool search --class \"" + browserClass + 
                          "\" windowactivate 2>/dev/null";
        std::system(cmd.c_str());
    }
}
#endif

void openUrl(const std::string& url) {
#ifdef _WIN32
    ShellExecuteA(NULL, "open", url.c_str(), NULL, NULL, SW_SHOWNORMAL);
#else
    // 先打开 URL
    std::string command = "xdg-open " + url;
    std::system(command.c_str());

    // 尝试将浏览器窗口提到前台
    bringBrowserToFront();
#endif
}