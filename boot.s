;------------------------------------------------------------------------------;
; Bootloader
;------------------------------------------------------------------------------;
                ;org        0x7c00               ; BIOS loads at this address
                bits       16                   ; 16 bits real mode
                jmp start ; skip data block
cpu_halted:
                db 10, "CPU halted!", 10, 0
read_fail:
                db 10, "Error loading sector", 10, 0
start:
                cli                             ; disable interrupts
                mov ax, 0x07c0                  ; setup stack (idk what this does)
                mov ds, ax                      ;
                mov gs, ax                      ;
                mov fs, ax                      ;
                mov es, ax                      ;
                mov ax, 0x07e0                  ;
                mov ss, ax                      ;
                mov bp, ax                      ; base pointer
                mov sp, 0xff                    ; stack pointer
                sti                             ; enable interrupts

                ; dl contains the drive number we booted from

                xor ah, ah        ;mov ah, 0x00 ; init
                int 13h

                ; load the next two sectors from disk right after the bootloader
                mov ah, 0x02
                mov al, 0x02                      ; read 2 sectors
                xor ch, ch          ;mov ch, 0x00 ; track 0
                mov cl, 0x02                      ; sector 2
                xor dh, dh          ;mov dh, 0x00 ; head 0
                mov bx, ds
                mov es, bx                        ; read into our current ds
                mov ebx, endofbootloader
read_loop:
                int 13h
                ;jc read_loop ; just retry until success FIXME:
                jc read_error
                
main_done:
                call brainfuck

halt:           mov si, cpu_halted
                call puts
.halt_loop:     cli                             ; disable interrupts
                hlt                             ; halt
                jmp .halt_loop                  ; if something managed to sneak by get right back to halting !
read_error:
                mov si, read_fail
                call puts
                jmp halt
;------------------------------------------------------------------------------;
;                                                                              ;
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
;                                                                              ;
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
;                                                                              ;
;------------------------------------------------------------------------------;
putc: ; char in al
                push ax ; +44
                mov ah, 0x0E ; print char
                cmp al, 0x0A ; \n
                jne .done
                mov al, 0x0D
                int 0x10
                mov al, 0x0A
.done:
                int 0x10
                pop ax
                ret

;------------------------------------------------------------------------------;
; from https://gist.github.com/inaz2/72de0fa471b207cb7a6b
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
                db "[-]+[[-],[.,]+]" ; Basic ECHO programm
                ;db "[-],----------[++++++++++.,----------]++++++++++.@" ; Echo programm terminates when enter is pressed
                db 0
;------------------------------------------------------------------------------;
; Bootloader signature must be located at offest 511 - 512                     ;
; Fill the remaining space with 0x00                                           ;
; $  = address of the current byte                                             ;
; $$ = address of the first byte                                               ;
;------------------------------------------------------------------------------;
                times 510 - ($-$$) db 0
mem:
                dw        0xaa55
endofbootloader:
