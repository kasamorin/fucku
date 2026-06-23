pkgname=fucku
pkgver=1.1
pkgrel=3
pkgdesc="A prank program that replaces fastfetch config with a custom one, with uninstall option"
arch=('x86_64' 'aarch64')
url="https://github.com/kasamorin/fucku"
license=('GPL3')
depends=('gcc-libs' 'glibc' 'xdotool')
source=("fucku.cpp")
md5sums=('SKIP')
options=('!debug')

build() {
    cd "$srcdir"
    g++ -O2 -std=c++17 -Wall -o fucku openUrl.cpp fucku.cpp
}

package() {
    cd "$srcdir"
    install -Dm755 fucku "$pkgdir/usr/bin/fucku"
}
