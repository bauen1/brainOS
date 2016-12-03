; From https://gist.github.com/YouriAckx/a52010a05a430496ae11

;----------------------------------------------;
;
;----------------------------------------------;
                ;org        0x7c00               ; BIOS loads at this address
                bits       16                   ; 16 bits real mode
                jmp main ; skip data block
; Print a welcome message.
; We have no DOS nor Linux kernel here.
; Therefore, we will use bios int 0x10.
main:
                cli                             ; disable interrupts
                mov ax, 0x07c0                  ; setup stack (idk what this does)
                mov ds, ax                      ;
                mov gs, ax                      ;
                mov fs, ax                      ;
                mov es, ax                      ;
                mov ax, 0x07e0                  ;
                mov ss, ax                      ;
                mov bp, ax                      ;
                mov sp, 0xff                    ;
                sti                             ; enable interrupts

                ; load the next two sectors from disk right after the bootloader
                ; dl contains the drive number we booted from
                ;mov ah, 2
                ;mov al, 2 ; number of sectors

;                mov cx, 3 ; number of read attempts
;read_loop:
;                xor ah, ah ; ah = 0; reset drive
;                int 13h
;
;                mov ax, ds
;                mov es, ax
;                mov bx, endofbootloader
;                ;mov dl, x ; drive number
;                mov dh, 0 ; HeadNumber
;                mov al, 1 ; NumSector
;                ;mov ch, CylNumHigh
;                ;mov cl, CylNumLow ; set the high part of the cylinder number, bits 6 and 7
;                ;and cl, Sector ; set sector number, bits 0 - 5
;                mov cx, 0x0001 ; cylinder 0 sector 1
;                mov ah, 2 ; func num. 2
;                int 13h
;                jnc .done
;                loop read_loop
;.done:
                mov ah, 0x02
                mov al, 0x01 ; read 1 sector
                mov ch, 0x00 ; track 0
                mov cl, 0x02 ; sector 2
                mov dh, 0x00 ; head 0
                mov bx, ds
                mov es, bx   ; read into our current ds
                mov ebx, endofbootloader
read_loop:
                int 13h
                jc read_loop


                ;mov al, '>'
                ;call putc
main_done:
                call brainfuck
;repl:           ; read execute p l
;                call getc
;                or al, al                       ; special char ?
;                jz repl
;                call putc
;                jmp repl

halt:           hlt                             ; halt

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

; from https://gist.github.com/inaz2/72de0fa471b207cb7a6b
brainfuck:
                mov edx, mem
                mov esi, bfcode

bfloop:
                mov al, [esi]
                cmp al, '>'
                je .inc
                cmp al, '<'
                je .dec
                cmp al, '+'
                je plus
                cmp al, '-'
                je minus
                cmp al, '.'
                je write
                cmp al, ','
                je read
                cmp al, '['
                je left
                cmp al, ']'
                je right
                jmp exit
.inc:
                inc edx
                jmp next

.dec:
                dec edx
                jmp next

plus:
                inc byte [edx]
                ;incbin [edx]
                jmp next

minus:
                dec byte [edx]
                jmp next

write:
                ;pushad
                ;mov ecx, edx
                ;mov edx, 1
                ;mov ebx, 1
                ;mov eax, 4
                ;int 0x80
                ;popad
                mov ax, [edx]
                call putc
                jmp next

read:
                ;pushad
                ;mov ecx, edx
                ;mov edx, 1
                ;mov ebx, 0
                ;mov eax, 3
                ;int 0x80
                ;popad
                call getc
                mov [edx], al
                jmp next

left:
                push esi
                jmp next

right:
                mov al, [edx]
                test al, al
                jz rightexit
                mov esi, [esp]
                jmp next
rightexit:
                pop eax
                jmp next

next:
                inc esi
                jmp bfloop

exit:
                xor ebx, ebx
                mov eax, 1
                ret
bfcode:

                db "[-]>[-]>[.>]<[[-]<]" ; Print all the data until 0x00 then erase it and return to start
                db ",[.,]" ; echo
                db 0

; brainfuck interpreter is 137 bytes long
;----------------------------------------------;
; Bootloader signature must be located
; at bytes #511 and #512.
; Fill with 0 in between.
; $  = address of the current line
; $$ = address of the 1st instruction
;----------------------------------------------;
                times 510 - ($-$$) db 0
mem:
                dw        0xaa55
endofbootloader:
