| start    | end        | size      | description    |
|----------|------------|-----------|----------------|
| 0x000000 | 0x000003FF | 1 kb      | Real Mode IVT  |
| 0x000400 | 0x000004FF | 256 bytes | BIOS data area |
| 0x000500 | 0x000005FF | 256 bytes | Free Memory    |
| 0x001000 | 0x00002FFF | 2kb       | stack          |
| 0x003000 | 0x00007BFF | 19kb      | Free Memory    |
| 0x007C00 | 0x00007DFF | 512 bytes | bootloader     |
| 0x007E00 | 0x0007FFFF | ~480.5 kb | Free Memory    |


Memory access:
segment*16+offset
