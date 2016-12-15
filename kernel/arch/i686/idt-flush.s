global idt_flush
idt_flush:        mov eax, [esp + 4] ; FIXME: maybe this should be 8 ?
                  lidt [eax]
                  ret
