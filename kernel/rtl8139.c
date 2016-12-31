#if !defined(__cplusplus)
#include <stdbool.h>
#endif

#include "rtl8139.h"
#include <stddef.h>
#include <stdint.h>
#include "pci.h"
#include "idt.h"
#include "system.h"

#define rtl8139_readb(offset) (inportb(rtl8139_data.io_addr + (offset)))
#define rtl8139_reads(offset) (inports(rtl8139_data.io_addr + (offset)))
#define rtl8139_readl(offset) (inportl(rtl8139_data.io_addr + (offset)))

#define rtl8139_writeb(offset, value) (outportb(rtl8139_data.io_addr + (offset), (value)))
#define rtl8139_writes(offset, value) (outports(rtl8139_data.io_addr + (offset), (value)))
#define rtl8139_writel(offset, value) (outportl(rtl8139_data.io_addr + (offset), (value)))
void rtl8139_init() {
}
