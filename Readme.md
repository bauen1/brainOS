# brainOS [![Build Status](https://travis-ci.org/bauen1/brainOS.svg?branch=master)](https://travis-ci.org/bauen1/brainOS) [![MIT Licence](https://badges.frapsoft.com/os/mit/mit.svg?v=103)](https://opensource.org/licenses/mit-license.php)   

![Screen](/screenshot.png)

# Dependencies
* NASM use your favorite package manager to get it
* [grub-mkrescue (linux has it preinstalled this is for macOS users)](http://wiki.osdev.org/GRUB_2#Installing_GRUB2_on_Mac_OS_X)

# Running
Please note that the kernel currently depends on grub2 loading it.

## Compile the Toolchain
* Compile it:
  ```
  make toolchain
  ```
* Install grub2
  * On Linux use `sudo apt-get install grub2`
  * On anything else, you have to install grub2 yourself

## Running in Qemu
* if grub-mkrescue is in your path: `make qemu`
* if grub-mkrescue is not in your path: `make qemu grub-mkrescue=/path/to/grub-mkrescue`

## Running on Real Hardware
* Generate the iso file: `make iso`
* Copy `image.iso` to a disk of your choice (using `dd`)
* Reboot and hope it doesn't blow up.
