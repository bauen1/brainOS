global tss_flush
tss_flush:
                  mov ax, 0x2B ; FIXME: why is this 0x2B and not 0x28, do some more reasearch on this
                  ;ltr ax ; TODO:
                  ret
