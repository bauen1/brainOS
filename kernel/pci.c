#include "pci.h"
#include "system.h"
#include "tty.h"
#include "vga.h"
#include <stddef.h>
#include <stdint.h>

const char * classcode_lookup[20] = {
  "Unclassified device",
  "Mass storage controller",
  "Network controller",
  "Display controller",
  "Mulimedia controller",
  "Memory controller",
  "Bridge",
  "Communication controller",
  "Generic system peripheral",
  "Input device controller",
  "Docking station",
  "Processor",
  "Serial bus controller",
  "Wireless controller",
  "Intelligent controller",
  "Satellite communication controller",
  "Encryption controller",
  "Signal processing controller",
  "Processing accelerators",
  "Non-Essential Instrumentation",
};

#define pci_get_address(bus, slot, func, offset) (0x80000000 | ((bus) << 16) | ((slot) << 11) | ((func) << 8) | ((offset) & 0xfc))

// TODO: make this such a handy function as below
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

void pci_scan(pci_scan_callback_t callback, void * driver_data) {
  for (uint16_t bus = 0; bus < 256; bus++) {
    for (uint8_t slot = 0; slot < 32; slot++) {
      uint16_t vendor_id = pci_checkVendor(bus, slot);
      if (vendor_id != 0xFFFF) {
        uint16_t device_id = pci_read_config(bus, slot, 0, 2, 2);
        callback(bus, slot, vendor_id, device_id, driver_data);
      }
    }
  }
}

/* This function is a example for a pci_scan callback
** In a driver you would perform a check to see if the device matches the one
** you've implemented and if so initialised it etc
*/
static void pci_printDevice(uint8_t bus, uint8_t slot, uint16_t vendor_id, uint16_t device_id, void * driver_data) {
  uint32_t head_field = pci_read_config(bus, slot, 0, 8, 4);
  uint8_t classcode = (uint8_t)((head_field & 0xFF000000) >> 24);
  uint8_t subclass = (uint8_t)((head_field & 0x00FF0000) >> 16);
  uint8_t prog_if = (uint8_t)((head_field & 0xFF00) >> 8);
  //uint8_t rev_id = (uint8_t)(head_field & 0xFF);

  uint8_t header_type = pci_read_config_header_type(bus, slot);
  uint8_t int_line = pci_read_config(bus, slot, 0, 0x3C, 1);

  puts("- ");
  const char * name;
  if (classcode <= 0x13) {
    name = classcode_lookup[classcode];
  } else if (classcode == 0x40) {
    name = "Coprocessor";
  } else if (classcode == 0xFF) {
    name = "Unassigned class";
  } else {
    name = "Reserved class";
  }

  size_t name_len = strlen(name);
  puts((char *)name);
  puts(" (0x");
  _puthex_8(classcode);
  puts(") -");
  for (size_t i = 0; i < (VGA_WIDTH - name_len - 11); i++) {
    putc('-');
  }
  puts("\n");

  kprintf("vendor_id:device_id:   %x:%x\n"
          "classcode: subclass:   %x:%x\n"
          "Prog IF:               0x%x\n"
          "header_type:           0x%x\t%s %s\n"
          "int_line:              0x%x\n",
          vendor_id, device_id,
          classcode, subclass,
          prog_if,
          header_type,
          (((header_type & 0x7F) == 0x00) ? "general" : ""),
          ((header_type & 0x80) ? "multifunction" : "general"),
          int_line
        );

  if ((header_type & 0x7F) == 0x00) {
    kprintf("bar0: %x\n", pci_read_config(bus, slot, 0, 0x10, 4));
    kprintf("bar1: %x\n", pci_read_config(bus, slot, 0, 0x14, 4));
    //kprintf("bar2: %x\n", pci_read_config(bus, slot, 0, 0x18, 4));
    //kprintf("bar3: %x\n", pci_read_config(bus, slot, 0, 0x1C, 4));
    //kprintf("bar4: %x\n", pci_read_config(bus, slot, 0, 0x20, 4));
    //kprintf("bar5: %x\n", pci_read_config(bus, slot, 0, 0x24, 4));
  } else if ((header_type & 0x7F) == 0x01) {
    kprintf("bar0: %x\n", pci_read_config(bus, slot, 0, 0x10, 4));
    kprintf("bar1: %x\n", pci_read_config(bus, slot, 0, 0x14, 4));
  }

  putc('\n');
}

void pci_list() {
  pci_scan((pci_scan_callback_t)pci_printDevice, NULL);
}
