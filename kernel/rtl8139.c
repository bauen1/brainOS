#if !defined(__cplusplus)
#include <stdbool.h>
#endif

#include "rtl8139.h"
#include <stddef.h>
#include <stdint.h>
#include "pci.h"
#include "idt.h"
#include "system.h"

static struct {
  uint32_t bar0;
  uint32_t bar1;
  uint8_t bus, slot;

  uint32_t io_addr;
  uint8_t mac[6];

  uint8_t rx_buffer[8208];
  uint8_t current_descriptor;
} rtl8139_data;

#define rtl8139_readb(offset) (inportb(rtl8139_data.io_addr + (offset)))
#define rtl8139_reads(offset) (inports(rtl8139_data.io_addr + (offset)))
#define rtl8139_readl(offset) (inportl(rtl8139_data.io_addr + (offset)))

#define rtl8139_writeb(offset, value) (outportb(rtl8139_data.io_addr + (offset), (value)))
#define rtl8139_writes(offset, value) (outports(rtl8139_data.io_addr + (offset), (value)))
#define rtl8139_writel(offset, value) (outportl(rtl8139_data.io_addr + (offset), (value)))

static void rtl8139_scan_callback(uint8_t bus, uint8_t slot, uint16_t vendor_id, uint16_t device_id, void * driver_data) {
  if ((vendor_id == 0x10ec) && (device_id == 0x8139)) {
    // Enable Bus mastering for our network card:
    uint32_t command = pci_read_config(bus, slot, 0, 0x04, 4);
    command |= (1 << 2) | (1 << 1) | (1 << 0);
    pci_config_write(bus, slot, 0x04, command);

    rtl8139_data.bus = bus;
    rtl8139_data.slot = slot;

    rtl8139_data.bar0 = pci_read_config(bus, slot, 0, 0x10, 4);
    rtl8139_data.bar1 = pci_read_config(bus, slot, 0, 0x10, 4);
    rtl8139_data.io_addr = rtl8139_data.bar0 & 0xFFFFFFFC;
  }
}

static void rtl8139_irq_handler(registers_t * registers) {
  puts("HELP ME\n");
  puthex("irq status reg:   ", rtl8139_reads(0x3E)); // this also clears the irq

  // XXX and fix qemu ?
  rtl8139_writes(0x3E, 0x01);
  return;
}

static void rtl8139_send(size_t size, uintptr_t bytes) {
  rtl8139_writel(0x10 + rtl8139_data.current_descriptor * 4, (uint32_t)bytes);
  //rtl8139_writel(0x20 + rtl8139_data.current_descriptor * 4, 0x3F0000 | (size & 0x3FFF));
  rtl8139_writel(0x20 + rtl8139_data.current_descriptor * 4, (uint32_t)size);

  puthex("a:      ", rtl8139_readl(0x10 + rtl8139_data.current_descriptor * 4));

  rtl8139_data.current_descriptor++;
  rtl8139_data.current_descriptor = rtl8139_data.current_descriptor % 4; // We only have 4 transmit descriptors
  return;
}

static void rtl8139_read_mac() {
  for (int i = 0; i < 6; i++) {
    rtl8139_data.mac[i] = rtl8139_readb(i);
  }
}

static bool rtl8139_reset() {
  rtl8139_writeb(0x52, 0);
  rtl8139_writeb(0x37, 0x10);
  for (int i = 0; i < 65500; i++) {
    if ((rtl8139_readb(0x37) & 0x10) == 0) {
      return true; // Successfully reset the card
    }
  }

  return false; // Failure
}

static bool rtl8139_irq_config() {
  uint8_t irq_line = pci_read_config(rtl8139_data.bus, rtl8139_data.slot, 0, 0x3C, 1) && 0xFF;
  irq_line = 11; // FIXME:

  if ((irq_line == 0xFF) | (irq_line > 15)) {
    puts("Falling back to hardcoded IRQ !!");
    irq_line = 11;
    uint32_t tmp = pci_read_config(rtl8139_data.bus, rtl8139_data.slot, 0, 0x3C, 4);
    tmp = (tmp & 0xFFFFFF00) | (irq_line & 0xFF);
    pci_config_write(rtl8139_data.bus, rtl8139_data.slot, 0, 0x3C, tmp);
  }

  puts("Registering IRQ handler: irq 0x");
  _puthex_8(irq_line);
  putc('\n');

  set_irq_handler(irq_line, rtl8139_irq_handler);
  return true;
}

void rtl8139_init() {
  pci_scan((pci_scan_callback_t)rtl8139_scan_callback, (void *)&rtl8139_data);

  if (rtl8139_data.io_addr != 0) {

    // 1. install the IRQ handler
    rtl8139_irq_config();

    // 2. Determine MAC address
    rtl8139_read_mac();

    puts("Mac address:      ");
    _puthex_8(rtl8139_data.mac[0]);putc(':');
    _puthex_8(rtl8139_data.mac[1]);putc(':');
    _puthex_8(rtl8139_data.mac[2]);putc(':');
    _puthex_8(rtl8139_data.mac[3]);putc(':');
    _puthex_8(rtl8139_data.mac[4]);putc(':');
    _puthex_8(rtl8139_data.mac[5]);
    putc('\n');

    // Reset the card
    if (rtl8139_reset()) {
      puts("Successfully reset the rt8139 card!\n");
    } else {
      puts("Something went wrong with reseting the rt8139 card, but we will continue anyway.\n");
    }

    // 3. Setup the receive buffer
    rtl8139_writeb(0x30, (uintptr_t)rtl8139_data.rx_buffer);

    // 4. Enable every IRQ i possibly can because why not
    // TODO:
    rtl8139_writes(0x3C,
      0x8000 | /* PCI error */
      0x4000 | /* PCS timeout */
      0x40   | /* Rx FIFO over */
      0x20   | /* Rx underrun */
      0x10   | /* Rx overflow */
      0x08   | /* Tx error */
      0x04   | /* Tx okay */
      0x02   | /* Rx error */
      0x01     /* Rx okay */
    );

    // TODO:
    rtl8139_writel(0x40, 0x00); // Configure transmiting

    // Configure the receive buffer
    // basically enable everything
    // TODO:
    rtl8139_writel(0x44,
      0x08 | 0x01
    );

    // Enable transmit and receive
    rtl8139_writeb(0x37, (1 << 3) | (1 << 2));

    rtl8139_writel(0x4C, 0x00); // No missed packets

    rtl8139_writel(0x8, 0xFFFFFFFF); // Setup multicast
    rtl8139_writel(0xC, 0xFFFFFFFF); // ^

    /*uint32_t tx_config_reg = rtl8139_readl(0x40);

    tx_config_reg &= 0xFFFEFFFF;

    tx_config_reg |= 0x00010000; // append a crc frame

    rtl8139_writel(0x40, tx_config_reg);*/





    rtl8139_data.current_descriptor = 0;

    uint8_t test[] = {
      0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, // Target MAC address
      rtl8139_data.mac[0], rtl8139_data.mac[1], rtl8139_data.mac[2], rtl8139_data.mac[3], rtl8139_data.mac[4], rtl8139_data.mac[5], // Our MAC address
      0x08, 0x06, // ARP Packet

      0x00, 0x01, // Hardware type: Ethernet
      0x08, 0x00, // Protocol type
      0x06, // Hardware size
      0x04, // Protocol size
      0x00, 0x01, // Opcode: request
      // Sender MAC address
      rtl8139_data.mac[0], rtl8139_data.mac[1], rtl8139_data.mac[2], rtl8139_data.mac[3], rtl8139_data.mac[4], rtl8139_data.mac[5], // Our MAC address

      // Sender IP:
      0x0a, 0x00, 0x00, 0x02,

      // Target MAC address:
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00,

      // Target IP address
      0x0a, 0x00, 0x00, 0x01,
      0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
      0x00, 0x00,
    };

    rtl8139_send(60, (uintptr_t)&test[0]);
  } else {
    puts("No rtl8139 card found\n");
  }
}
