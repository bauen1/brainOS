global tss_flush
tss_flush:
                  mov byte ax, [esp + 4]
                  ltr ax
                  ret
