#ifndef PCI_H
#define PCI_H

#include <stdint.h>
#include <stddef.h>

#define PCI_CONFIG_DATA 0x0CFC
#define PCI_CONFIG_ADDRESS 0x0CF8

void pci_list();

void pci_install();

typedef void (* pci_scan_callback_t)(uint8_t bus, uint8_t slot, uint16_t vendor_id, uint16_t device_id, void * driver_data);

void pci_scan(pci_scan_callback_t callback, void * driver_data);

uint32_t pci_read_config(uint8_t bus, uint8_t slot, uint8_t func, uint8_t offset, size_t size);

#define pci_read_config_header_type(bus, slot) ((uint8_t)pci_read_config((bus), (slot), 0, 0x0E, 1))
#define pci_checkVendor(bus, slot) (pci_read_config((bus), (slot), 0, 0, 2))

#endif
