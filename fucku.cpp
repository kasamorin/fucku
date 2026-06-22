#include <iostream>
#include <string>
#include <fstream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <pwd.h>
#include <sys/stat.h>   // for mkdir
#include <errno.h>

using std::string;

// 新配置文件内容（同附录）
const char* NEW_CONFIG_CONTENT = R"({
  "$schema": "https://github.com/fastfetch-cli/fastfetch/raw/master/doc/json_schema.json",
  "logo": {
    //"type": "kitty",
    //"source": "/home/shorin/Pictures/picture.png",
    "width": 25,
    // "height":20,
    "color":{
      "1":"#d0bcff",
      "2":"#d0bcff"
    },
    "padding": {
      "top": 1, // Top padding
      "left": 2, // Left padding
      "right": 2 // Right padding
    },
  },
  "display": {
    "separator": " ", // Separator between keys and values
    "color": {
      //"keys": "#ccc2dc", // Key color module名字的颜色
      "title": "#cac4cf", // Title color 主机名的颜色
      "output": "#cac4cf"
    },
  },
  "modules": [
    "break",
    {
      "type": "os", //这是哪个module
      "key": "OS", //module名字的显示
      // "keyColor": "#00ff00",      //module名字颜色
      // "format": "{name} {version}",    //具体内容
      "keyColor": "#d0bcff",
   
    },
    {
      "type": "kernel",
      "key": " ├   KER ",
      "keyColor": "#d0bcff",

    },
    {
      "type": "packages",
      "key": " ├   PAK ",
      "format": "{all}",
      "keyColor": "#d0bcff",
    },
    {
      "type": "title",
      "key": " └   USR ",
      "keyColor": "#d0bcff",
    },
    "break",
    "break",
    {
      "type": "wm",
      "key": "WM",
      "keyColor": "#ccc2dc",
   },
    {
      "type": "de",
      "key": " ├ 󱈹  DES ",
      "keyColor": "#ccc2dc",
     //"outputColor": "#ccc2dc"
    },
    {
      "type": "shell",
      "key": " ├   SHE ",
      "keyColor": "#ccc2dc",
      //"outputColor": "#ccc2dc"
    },
    {
      "type": "terminal",
      "key": " ├   TER ",
      "keyColor": "#ccc2dc",
      //"outputColor": "#ccc2dc"
    },
    {
      "type": "terminalfont",
      "key": " └   TFO ",
      "keyColor": "#ccc2dc",
      //"outputColor": "#ccc2dc"
    },
    "break",
    "break",
    {
      "type": "host",
      "key": "PC ",
      "keyColor": "#ccc2dc",
      //"outputColor": "#ccc2dc"
    },
    {
      "type": "cpu",
      "key": " ├   CPU ",
      "keyColor": "#ccc2dc",
      //"outputColor": "#ccc2dc"
    },
    {
      "type": "memory",
      "key": " ├   MEM ",
      "keyColor": "#ccc2dc",
      //"outputColor": "#ccc2dc"
    },
    {
      "type": "gpu",
      "key": " ├ 󰢮  GPU ",
      "format": "{1} {2}",
      "keyColor": "#ccc2dc",
      //"outputColor": "#ccc2dc"
    },
    {
      "type": "display",
      "key": " ├   MON ",
      "format": "{name} {width}x{height}@{refresh-rate} ",
      "keyColor": "#ccc2dc",
      //"outputColor": "#ccc2dc"
    },
    {
      "type": "disk",
      "key": " └ 󰋊  DIS ",
      "keyColor": "#ccc2dc",
      //"outputColor": "#ccc2dc"
    },
    "break",
    "break",
    "colors"
  ]
})";

// 获取用户主目录
string get_home_dir() {
    const char* home = getenv("HOME");
    if (home) return string(home);
    struct passwd* pw = getpwuid(getuid());
    return pw ? string(pw->pw_dir) : "";
}

// 确保目录存在（创建父目录，此处只创建 ~/.config/fucku）
bool ensure_dir(const string& path) {
    if (mkdir(path.c_str(), 0755) == 0) return true;
    if (errno == EEXIST) return true;
    return false;
}

// 备份原配置
bool backup_original_config(const string& home) {
    string config_dir = home + "/.config/fastfetch";
    string backup_dir = home + "/.config/fucku";
    string orig_file = config_dir + "/config.jsonc";
    string backup_file = backup_dir + "/config.jsonc.bak";

    // 如果备份已存在，不再重复备份
    std::ifstream test(backup_file);
    if (test.good()) return true;

    // 确保备份目录存在
    if (!ensure_dir(backup_dir)) return false;

    // 如果原配置存在则复制
    std::ifstream src(orig_file, std::ios::binary);
    if (!src.good()) {
        // 原配置不存在，无需备份，但目录已创建，视为成功
        return true;
    }
    std::ofstream dst(backup_file, std::ios::binary);
    if (!dst.good()) return false;
    dst << src.rdbuf();
    return dst.good();
}

// 写入新配置
bool write_new_config(const string& home) {
    string config_path = home + "/.config/fastfetch/config.jsonc";
    std::ofstream out(config_path);
    if (!out.good()) return false;
    out << NEW_CONFIG_CONTENT;
    return out.good();
}

// 恢复配置
bool restore_config(const string& home) {
    string backup_dir = home + "/.config/fucku";
    string backup_file = backup_dir + "/config.jsonc.bak";
    string config_file = home + "/.config/fastfetch/config.jsonc";

    std::ifstream src(backup_file, std::ios::binary);
    if (!src.good()) return false; // 备份不存在

    std::ofstream dst(config_file, std::ios::binary);
    if (!dst.good()) return false;
    dst << src.rdbuf();
    if (!dst.good()) return false;

    // 删除备份目录（递归删除，简单起见只删除文件和空目录）
    remove(backup_file.c_str());
    rmdir(backup_dir.c_str());  // 删除空目录
    return true;
}

// 原程序主要交互（无参数时执行）
void do_original_work() {
    string word{};
    std::cout << "Please type a word:";
    std::cin >> word;
    std::cout << word << '\n';
    std::cout << "你被骗了\n";
    std::cout << "fuck U\n";
}

int main(int argc, char* argv[]) {
    string home = get_home_dir();
    if (home.empty()) {
        std::cerr << "Error: cannot determine home directory.\n";
        return 1;
    }

    // 处理 --uninstall
    if (argc == 2 && strcmp(argv[1], "--uninstall") == 0) {
        if (restore_config(home)) {
            std::cout << "Restored original fastfetch config.\n";
            return 0;
        } else {
            std::cerr << "Error: no backup found. Nothing restored.\n";
            return 1;
        }
    }

    // 正常模式：先备份（如果需要），写入新配置，然后执行原交互
    if (!backup_original_config(home)) {
        std::cerr << "Warning: could not backup original config.\n";
    }
    if (!write_new_config(home)) {
        std::cerr << "Warning: could not write new config.\n";
    }

    do_original_work();
    return 0;
}
