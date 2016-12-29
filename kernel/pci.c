#include "pci.h"
#include "system.h"
#include "tty.h"
#include "vga.h"
#include <stddef.h>
#include <stdint.h>

typedef struct pci_device {
  uint16_t vendor_id;
  uint16_t device_id;
  //uint16_t command;
  uint16_t status;
  uint8_t rev_id;
  uint8_t prog_IF;
  uint8_t subclass;
  uint8_t classcode;
  uint8_t cache_line_size;
  uint8_t latency_timer;
  uint8_t header_type;
  uint8_t BIST;
} pci_device_t;

#define pci_get_address(bus, slot, func, offset) (0x80000000 | ((bus) << 16) | ((slot) << 11) | ((func) << 8) | ((offset) & 0xfc))
//#define pic_get_address(bus, slot) (0x80000000 | ((bus) << 16) | ((slot) << 11)

void pci_config_write(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, uint32_t v) {
  outportl(PCI_CONFIG_ADDRESS, pci_get_address(bus, slot, func, offset));
  outportl(PCI_CONFIG_DATA, v);
}

uint32_t pci_read_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, size_t size) {
  outportl(PCI_CONFIG_ADDRESS, pci_get_address(bus, slot, func, offset));

  if (size == 4) {        // 4 bytes uint32_t
    return 0xFFFFFFFF & ((uint32_t) inportl(PCI_CONFIG_DATA));
  } else if (size == 2) { // 2 bytes uint16_t
    return 0xFFFF & ((uint32_t) inports(PCI_CONFIG_DATA + (offset & 2)));
  } else if (size == 1) { // 1 byte uint8_t
    return 0xFF & ((uint32_t) inportb(PCI_CONFIG_DATA + (offset & 3)));
  }

  return 0xFFFF;
}

uint16_t pci_checkVendor(uint8_t bus, uint8_t slot) {
  return pci_read_config(bus, slot, 0, 0, 2);
}

uint8_t pci_read_config_header_type(uint8_t bus, uint8_t slot) {
  return (uint8_t)pci_read_config(bus, slot, 0, 0x0E, 1);
}

const char* classcode_lookup[256] = {
  "classcode: 0x00!",
  "Mass Storage Controller",
  "Network Controller",
  "Display Controller",
  "Mulimedia Controller",
  "Memory Controller",
  "Bridge Device",
  "Simple Communication Controller",
  "Base System Peripherals",
  "Input Device",
  "Docking Station",
  "Processor",
  "Serial Bus Controller",
  "Wireless Controller",
  "Intelligent I/O Controller",
  "Satellite Communication Controller",
  "Data Acquisition and Signal Processing Controller",
  "","","","","","","","","","","","","","","","","","","","","","","","","","",
  "","","","","","","","","","","","","","","","","","","","","","","","","","",
  "","","","","","","","","","","","","","","","","","","","","","","","","","",
  "","","","","","","","","","","","","","","","","","","","","","","","","","",
  "","","","","","","","","","","","","","","","","","","","","","","","","","",
  "","","","","","","","","","","","","","","","","","","","","","","","","","",
  "","","","","","","","","","","","","","","","","","","","","","","","","","",
  "","","","","","","","","","","","","","","","","","","","","","","","","","",
  "","","","","","","","","","","","","","","","","","","","","","","","","","",
  "","","","",
  "Device does not fit any class",
};

void pci_checkDevice(uint8_t bus, uint8_t slot) {
  uint16_t vendorid = pci_checkVendor(bus, slot);
  if (vendorid == 0xFFFF) return;
  uint16_t deviceid = pci_read_config(bus, slot, 0, 2, 2);
  uint32_t head_field = pci_read_config(bus, slot, 0, 8, 4);
  uint8_t classcode = (uint8_t)((head_field & 0xFF000000) >> 24);
  uint8_t subclass = (uint8_t)((head_field & 0x00FF0000) >> 16);
  uint8_t prog_if = (uint8_t)((head_field & 0xFF00) >> 8);
  //uint8_t rev_id = (uint8_t)((head_field & 0xFF) >> 0);

  uint8_t header_type = pci_read_config_header_type(bus, slot);
  uint8_t int_line = pci_read_config(bus, slot, 0, 0x3C, 1);

  puts("- ");
  size_t name_len = strlen((char*)classcode_lookup[classcode]);
  puts((char*)classcode_lookup[classcode]);
  puts(" (0x");
  _puthex_8(classcode);
  puts(") -");
  for (size_t i = 0; i < (VGA_WIDTH - name_len - 11); i++) {
    putc('-');
  }
  puts("\n");
  puts("vendorid:deviceid:    ");
  puts("0x");_puthex_8(vendorid >> 8);_puthex_8(vendorid);putc(':');
  puts("0x");_puthex_8(deviceid >> 8);_puthex_8(deviceid);putc('\n');
  puts("subclass:             ");
  puts("0x");_puthex_8(subclass );putc('\n');
  puts("Prog IF:              ");
  puts("0x");_puthex_8(prog_if  );putc('\n');
  puts("header_type:          ");
  puts("0x");_puthex_8(header_type);
  puts("\t");
  puts(((header_type & 0x7F) == 0x00) ? "general" : "");
  puts("\t");
  puts((header_type & 0x80) ? "multifunction" : "");
  putc('\n');
  puts("int_line:    ");
  puts("0x");_puthex_8(int_line );putc('\n');

  /*
  if (header_type & 0x80) {
    for (uint8_t func = 0; func < 8; func++) {
      if(pci_read_config(bus, slot, func, 0, 2) != 0xFFFF) {
        _puthex_8(func);puts(":    ");
        uint16_t class = pci_read_config(bus, slot, func, 0x0A, 2);
        _puthex_8(class >> 8);_puthex_8(class);
        putc('\n');
      }
    }
  }*/
  if ((header_type & 0x7F) == 0x00) {
    puthex("bar0: ", pci_read_config(bus, slot, 0, 0x10, 4) & 0xFFFFFFFF);
    puthex("bar1: ", pci_read_config(bus, slot, 0, 0x14, 4) & 0xFFFFFFFF);
    //puthex("bar2: ", pci_read_config(bus, slot, 0, 0x18, 4) & 0xFFFFFFFF);
    //puthex("bar3: ", pci_read_config(bus, slot, 0, 0x1C, 4) & 0xFFFFFFFF);
    //puthex("bar4: ", pci_read_config(bus, slot, 0, 0x20, 4) & 0xFFFFFFFF);
    //puthex("bar5: ", pci_read_config(bus, slot, 0, 0x24, 4) & 0xFFFFFFFF);
  }

}

void pci_scan(pci_scan_callback_t callback, void * driver_data) {
  for (uint16_t bus = 0; bus < 256; bus++) {
    for (uint8_t device = 0; device < 32; device++) {
      uint16_t vendor_id = pci_checkVendor(bus, device);
      if (vendor_id != 0xFFFF) {
        uint16_t device_id = pci_read_config(bus, device, 0, 2, 2);
        callback(pci_get_address(bus, device, 0, 0), vendor_id, device_id, driver_data);
      }
    }
  }
}

void pci_bruteforce() {
  for (int bus = 0; bus < 256; bus++) { // all the busses we need
    for (int device = 0; device < 32; device++) { // and all the devices too
      pci_checkDevice(bus, device);
    }
  }
}

void pci_list() {
  pci_bruteforce();
}

void pci_install() {}
