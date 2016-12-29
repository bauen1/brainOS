global inportb
inportb:

mov edx, [esp + 4]
in ax, dx
ret

global outportb
outportb:

mov eax, [esp + 8]
mov edx, [esp + 4]
out dx, al
ret

global inports
inports:
mov edx, [esp + 4]
in ax, dx
ret

global outports
outports:
mov eax, [esp + 8]
mov edx, [esp + 4]
out dx, ax
ret

global inportl
inportl:
mov edx, [esp + 4]
in eax, dx
ret

global outportl
outportl:
mov eax, [esp + 8]
mov edx, [esp + 4]
out dx, eax
