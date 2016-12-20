#!/bin/bash
PREFIX="$PWD/opt/cross"
TARGET=i686-elf
PATCH_PATH="$PWD"
export PATH="$PREFIX/bin:$PATH"

mkdir -p "$HOME/build/"

function get() {
  echo "Getting" $1 "from $2/$3"
  if [ ! -f "$3" ]; then
    wget -q "$2/$3"
  else
    echo "Skipping ..."
  fi
}

# Someone please help me with signature checking xD

#mkdir -p "$PWD/build/automake"
#pushd .
#  cd "$PWD/build/automake"
#  get "automake-1.11.1" "http://ftp.gnu.org/gnu/automake" "automake-1.11.1.tar.bz2" || exit 1
#  tar -xf "automake-1.11.1.tar.bz2" || exit 2
#  ./automake-1.11.1/configure --prefix="$PREFIX" || exit 3
#  make || exit 4
#  make install || exit 5
#popd

#mkdir -p "$PWD/build/autoconf"
#pushd .
#  cd "$PWD/build/autoconf"
#  get "autoconf-2.64" "http://ftp.gnu.org/gnu/autoconf" "autoconf-2.64.tar.bz2" || exit 1
#  tar -xf "autoconf-1.11.1.tar.bz2" || exit 2
#  ./autoconf-2.64/configure --prefix="$PREFIX" || exit 3
#  make || exit 4
#  make install || exit 5
#popd

#export PATH_OLD=$PATH
#export PATH="$HOME/opt/cross/bin:$PATH"

if [ -e "$PREFIX/bin/$TARGET-objcopy" ]
then
  echo "skipping binutils"
else
  mkdir -p "$PWD/build/binutils"
  pushd .
    cd "$PWD/build/binutils/"
    get "binutils2.24" "http://ftp.gnu.org/gnu/binutils" "binutils-2.24.tar.gz" || exit 1
    tar -xf "binutils-2.24.tar.gz" || exit 2
    echo "Applying patch for os-specific toolchain to binutils"
    patch -p 2 < "$PATCH_PATH/binutils-2.24-brainos.patch"
    ./binutils-2.24/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror || exit 3
    make || exit 4
    make install || exit 5
  popd

fi

if [ -e "$PREFIX/bin/$TARGET-gcc" ]
then
  echo "skipping gcc"
else
  mkdir -p "$PWD/build/gcc/"
  pushd .
    cd "$PWD/build/gcc/"
    get "gcc6.2.0" "http://ftp.gnu.org/gnu/gcc/gcc-6.2.0" "gcc-6.2.0.tar.gz" || exit 1
    tar -xf "gcc-6.2.0.tar.gz" || exit 2
    echo "Applying patch for os-specific toolchain to gcc"
    patch -p 2 < "$PATCH_PATH/gcc-6.2.0-brainos.patch"
    ./gcc-6.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers || exit 3
    make all-gcc || exit 4
    make all-target-libgcc || exit 5
    make install-gcc || exit 6
    make install-target-libgcc || exit 7
  popd
fi

#mkdir -p "$PWD/build/flex/"
#pushd .
#  cd "$PWD/build/flex"
#  get "flex2.6.2" "https://github.com/westes/flex/releases/download/v2.6.2" "flex-2.6.2.tar.gz" || exit 1
#  tar -xf "flex-2.6.2.tar.gz" || exit 2
#  ./flex-2.6.2/configure --prefix=$PREFIX # Hey this shouldn't be installed there but i don't care \o/
#  make || exit 4
#  make install || exit 5
#popd .

#mkdir -p "$PWD/build/grub2/"
#pushd .
#  cd "$PWD/build/grub2/"
#  get "grub2.00" "ftp://ftp.gnu.org/gnu/grub" "grub-2.00.tar.xz" || exit 1
#  tar -xf "grub-2.00.tar.xz" || exit 2
#  ./grub-2.00/configure --disable-werror TARGET_CC="$PREFIX/bin/$TARGET-gcc" TARGET_OBJCOPY="$PREFIX/bin/$TARGET-objcopy" TARGET_STRIP="$PREFIX/bin/$TARGET-strip" TARGET_NM="$PREFIX/bin/$TARGET-nm" --target=$TARGET --prefix=$PREFIX || exit 3
#  make || exit 4
#  make install || exit 5
#popd

echo "Skipping grub2 install please provide your own grub-mkrescue for the moment"

printf "\n\aFinished\n"
