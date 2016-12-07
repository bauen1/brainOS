;------------------------------------------------------------------------------;
; Bootloader
;------------------------------------------------------------------------------;
                org        0x00007c00               ; BIOS loads at this address
                bits       16                   ; 16 bits real mode
                jmp start ; skip data block
cpu_halted:
                db "CPU halted!", 10, 0
read_fail:
                db "Error loading sector", 10, 0
start:
                xor ax, ax                      ; setup all the following segments to 0000:xxxx
                mov ds, ax                      ; data segment
                mov es, ax                      ; extra segment
                mov fs, ax                      ; 2nd extra segment
                mov gs, ax                      ; 3rd extra segment

                mov bp, 0x5000                  ; stack grows down

                cli                             ; disable interrupts

                mov ss, ax                      ; from  0x7c00
                mov sp, 0x7c00                  ; to    0x5000

                sti                             ; enable interrupts

                ; dl contains the drive number we booted from

                call reset_drive

                ; load the next two sectors from disk right after the bootloader
                mov ah, 0x02
                mov al, 0x03                      ; read 3 sectors
                xor ch, ch                        ; track 0
                mov cl, 0x02                      ; sector 2
                xor dh, dh                        ; head 0
                mov bx, ds
                mov es, bx                        ; read into our current ds
                mov ebx, endofbootloader          ; at the end of our current bootloader
read_loop:
                int 13h
                jc read_error                     ; just retry until success FIXME:

main_done:
                call brainfuck

halt:           mov si, cpu_halted
                call puts
.halt_loop:     cli                             ; disable interrupts
                hlt                             ; halt
                jmp .halt_loop                  ; if something managed to sneak by get right back to halting !
read_error:
                call reset_drive
                mov si, read_fail
                call puts
                jmp halt

reset_drive:
                xor ah, ah                        ; init drive
                int 13h
                ret

;------------------------------------------------------------------------------;
; puts:                                                                        ;
;------------------------------------------------------------------------------;
puts:
                push ax
.loop:
                lodsb                           ; load byte at SI into al incrementing SI
                cmp al, 0x00
                je .done
                call putc
                jmp .loop
.done:          pop ax
                ret
;------------------------------------------------------------------------------;
; getc:                                                                        ;
;------------------------------------------------------------------------------;
getc: ; returns pressed key in al, keycode in ah
                xor ah, ah                      ; ah = 0
                int 16h
                cmp al, 0x0D                    ; '\r' convert \r to \n
                jne .done
                mov al, 0x0A                    ; '\n'
.done:
                or al, al                       ; special char ?
                jz getc                         ; just get the next one :D
                ret

;------------------------------------------------------------------------------;
; putc:                                                                        ;
;------------------------------------------------------------------------------;
putc: ; char in al
                push ax
                push bx
                xor bx, bx
                mov ah, 0x0E                    ; print char
                cmp al, 0x0A                    ; \n
                jne .done
                mov al, 0x0D
                int 0x10                        ;
                mov al, 0x0A
.done:
                int 0x10                        ;
                pop bx
                pop ax
                ret

;------------------------------------------------------------------------------;
; Bootloader signature must be located at offest 511 - 512                     ;
; Fill the remaining space with 0x00                                           ;
; $  = address of the current byte                                             ;
; $$ = address of the first byte                                               ;
;------------------------------------------------------------------------------;
                times 510 - ($-$$) db 0
magic:
                dw        0xaa55
endofbootloader:
;------------------------------------------------------------------------------;
; END OF BOOTLOADER
;------------------------------------------------------------------------------;

;------------------------------------------------------------------------------;
; brainfuck:                                                                   ;
; from https://gist.github.com/inaz2/72de0fa471b207cb7a6b                      ;
;------------------------------------------------------------------------------;
brainfuck:
                mov edx, mem
                mov esi, bfcode
                jmp .bfloop

.next:
                inc esi
.bfloop:
                mov al, [esi]
                cmp al, '>'
                je .inc
                cmp al, '<'
                je .dec
                cmp al, '+'
                je .plus
                cmp al, '-'
                je .minus
                cmp al, '.'
                je .write
                cmp al, ','
                je .read
                cmp al, '['
                je .left
                cmp al, ']'
                je .right
                cmp al, "@"
                je .exit
                cmp al, 0x00
                jne .bfloop
                ; terminating 0 found, please exit now
.exit:          xor ebx, ebx
                mov eax, 1
                ret
.inc:
                inc edx
                jmp .next

.dec:
                dec edx
                jmp .next

.plus:
                inc byte [edx]
                jmp .next

.minus:
                dec byte [edx]
                jmp .next

.write:
                mov ax, [edx]
                call putc
                jmp .next

.read:
                call getc
                mov [edx], al
                jmp .next

.left:
                push esi
                ; TODO: My Loop skips skips a beat (skip loop if cell == 0)
                mov al, [edx]
                test al, al
                jnz .next ; cell != 0

                push ebx
                mov ebx, 0x0001

.nextsearch:    inc esi
.searchloop:
                mov al, [esi]
                cmp al, '['
                je .searchopen
                cmp al, ']'
                je .searchclose
                cmp al, 0x00 ; FIXME: maybe we should remove this
                je .exit
                jmp .nextsearch
.searchopen:
                inc ebx
                je .searchfinish ; "stack" underflow
                jmp .nextsearch
.searchclose:
                dec ebx
                jne .nextsearch
                ; fall through
.searchfinish:  pop ebx
                jmp .next

.right:
                mov al, [edx]
                test al, al
                jz .rightexit
                mov esi, [esp]
                jmp .next
.rightexit:
                pop eax
                jmp .next

bfcode:
                ;db "[-]>[-]<[>[-]++++++++++<[-]]>.[-]<" ; test for loop skiping, prints a newline if loops aren't skipped if the current cell is 0
                ;db "[-]+[[-],[.,]+]" ; Basic ECHO programm
                ;db "[-],----------[++++++++++.,----------]++++++++++.@" ; Echo programm terminates when enter is pressed
                db ",[.,]"
                db 0
mem:
