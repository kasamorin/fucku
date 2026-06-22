# Fuck U

Fuck U 是一款由Morin指导，由DeepSeek-v4-flash制作的一款ArchLinux软件包</br>
Fuck U 致力于（并非) 统一每个人的fastfetch配置，只要你安装了fastfetch且配置地址位于~/.config/config.jsonc,那么安装后运行fucku并不要中途终止就可以更改你的fastfetch配置</br>
fastfetch配置来自网络</br>
<i>本项目为整活性质，请勿深究</i>
# 安装方法
1.手动安装</br>
```bash
git clone https://github.com/kasamorin/fucku.git
cd fucku
makepkg -si
```
2.更加手动的安装</br>
```bash
git clone https://github.com/kasamorin/fucku.git
cd fucku
g++ fucku.cpp -o fucku
sudo cp fucku /usr/bin
```
