# Maintainer: Name <me@example.com>
pkgname=qsh
pkgver=1.0
pkgrel=1
pkgdesc="Quantum Shell (qsh) - simple custom shell in C"
arch=('x86_64')
url='https://github.com/fahim-foysal-097/QuantumShell'
license=('MIT')
depends=()
makedepends=('gcc' 'make')
source=()
md5sums=()

build() {
    cd "$srcdir"
    # make
}

package() {
    # cd "$srcdir"
    cp /mnt/files/dev/C/QuantumShell/qsh "$srcdir/"
    install -Dm755 qsh "$pkgdir/usr/bin/qsh"
    # optional: install default config
    # install -Dm644 qshrc "$pkgdir/etc/qshrc"
}
