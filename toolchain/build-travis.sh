#!/bin/bash
PREFIX="$HOME/opt/cross"
TARGET=i686-elf
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

if [ -e "$PREFIX/bin/$TARGET-objcopy" ]
then
  echo "skipping binutils"
else
  mkdir -p "$HOME/build/binutils"
  pushd .
    cd "$HOME/build/binutils/"
    get "binutils2.27" "http://ftp.gnu.org/gnu/binutils" "binutils-2.27.tar.gz" || exit 1
    tar -xf "binutils-2.27.tar.gz" || exit 2
    ./binutils-2.27/configure --target=$TARGET --prefix="$PREFIX" --with-sysroot --disable-nls --disable-werror || exit 3
    make || exit 4
    make install || exit 5
  popd

fi

if [ -e "$PREFIX/bin/$TARGET-gcc" ]
then
  echo "skipping gcc"
else
  mkdir -p "$HOME/build/gcc/"
  pushd .
    cd "$HOME/build/gcc/"
    get "gcc6.2.0" "http://mirrors.cicku.me/gnu/gcc/gcc-6.2.0/" "gcc-6.2.0.tar.gz" || exit 1
    tar -xf "gcc-6.2.0.tar.gz" || exit 2
    ./gcc-6.2.0/configure --target=$TARGET --prefix="$PREFIX" --disable-nls --enable-languages=c,c++ --without-headers || exit 3
    make all-gcc || exit 4
    make all-target-libgcc || exit 5
    make install-gcc || exit 6
    make install-target-libgcc || exit 7
  popd
fi

#mkdir -p "$HOME/build/flex/"
#pushd .
#  cd "$HOME/build/flex"
#  get "flex2.6.2" "https://github.com/westes/flex/releases/download/v2.6.2" "flex-2.6.2.tar.gz" || exit 1
#  tar -xf "flex-2.6.2.tar.gz" || exit 2
#  ./flex-2.6.2/configure --prefix=$PREFIX # Hey this shouldn't be installed there but i don't care \o/
#  make || exit 4
#  make install || exit 5
#popd .

#mkdir -p "$HOME/build/grub2/"
#pushd .
#  cd "$HOME/build/grub2/"
#  get "grub2.00" "ftp://ftp.gnu.org/gnu/grub" "grub-2.00.tar.xz" || exit 1
#  tar -xf "grub-2.00.tar.xz" || exit 2
#  ./grub-2.00/configure --disable-werror TARGET_CC="$PREFIX/bin/$TARGET-gcc" TARGET_OBJCOPY="$PREFIX/bin/$TARGET-objcopy" TARGET_STRIP="$PREFIX/bin/$TARGET-strip" TARGET_NM="$PREFIX/bin/$TARGET-nm" --target=$TARGET --prefix=$PREFIX || exit 3
#  make || exit 4
#  make install || exit 5
#popd

echo "Skipping grub2 install please provide your own grub-mkrescue for the moment"

printf "\n\aFinished\n"
