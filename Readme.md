# brainOS [![Build Status](https://travis-ci.org/bauen1/brainOS.svg?branch=master)](https://travis-ci.org/bauen1/brainOS)

[![Screen](https://raw.github.com/bauen1/brainOS/screenshot.png)](https://raw.github.com/bauen1/brainOS/screenshot.png)

# Dependencies
* [Cross-compiling GCC](http://wiki.osdev.org/GCC_Cross-Compiler)
* [grub-mkrescue (linux has it preinstalled this is for macOS users)](http://wiki.osdev.org/GRUB_2#Installing_GRUB2_on_Mac_OS_X)

# Compiling

## Compile the Toolchain
* Compile it:
  ```
  make toolchain
  ```
* Install grub2-mkresuce yourself until i figure out how i installed it, take a look at `toolchain/build.sh`
## Running in Qemu
```
make qemu
```
