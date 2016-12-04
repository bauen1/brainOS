# brainOS - a brainfuck executing os

# Linux / Unix only at the moment
Compiling:
```
make all
```
This generates a file "image" which can be used by a VM as raw disk / floppy

If you have qemu installed:
```
make test
```
Will launch a qemu machine with BrainOS
