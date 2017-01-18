#include "initrd.h"
#include "fs.h"
#include "string.h"
#include <stdint.h>

typedef struct cpio_newc_header {
  char c_magic[6];
  char c_ino[8];
  char c_mode[8];
  char c_uid[8];
  char c_gid[8];
  char c_nlink[8];
  char c_mtime[8];
  char c_filesize[8];
  char c_devmajor[8];
  char c_devminor[8];
  char c_rdevmajor[8];
  char c_rdevminor[8];
  char c_namesize[8];
  char c_check[8];
} __attribute__((packed)) cpio_newc_header_t;

static inline uint32_t parse_octal(uint8_t * chars) {
  uint32_t size = 0;
  uint32_t count = 1;
  for (int j = 8; j > 0; j--, count *= 8) {
    size += ((chars[j - 1] - '0') * count);
  }
  return size;
}

static void cpio_parse(void * addr) {
  kprintf("addr: 0x%x\n", addr);
  cpio_newc_header_t * header = (cpio_newc_header_t *)(&addr);

  for (uint32_t i = 0; ; i++) {
    kprintf(
      "ino:             0x%x%x%x%x%x%x%x%x\n"
      "mode:            0x%x\n"
      "uid:             0x%x\n"
      "gid:             0x%x\n"
      "nlink:           0x%x\n"
      "mtime:           0x%x\n"
      "filesize:        0x%x%x%x%x%x%x%x%x\n"
      "namesize:        0x%x\n"
      "check:           0x%x\n",
      header->c_ino[0],header->c_ino[1],header->c_ino[2],header->c_ino[3],header->c_ino[4],header->c_ino[5],header->c_ino[6],header->c_ino[7],
      header->c_mode,
      header->c_uid,
      header->c_gid,
      header->c_nlink,
      header->c_mtime,
      header->c_filesize[0],header->c_filesize[1],header->c_filesize[2],header->c_filesize[3],header->c_filesize[4],header->c_filesize[5],header->c_filesize[6],header->c_filesize[7],
      header->c_namesize[0],
      header->c_check
    );

    uint32_t namesize = parse_octal(header->c_namesize);
    uint32_t filesize = parse_octal(header->c_filesize);
    char * name = (char *)(header + sizeof(cpio_newc_header_t));

    kprintf("namesize: %d\nfilesize: 0x%x\n", namesize, filesize);

    puts(name);
    putc('\n');

    if(strncmp(name, "TRAILER!!!", 11) == 0) {
      break;
    }

    if(strncmp(header->c_magic, "070701", 6) != 0) {
      break;
    }

    header = &header + sizeof(cpio_newc_header_t) + namesize + filesize;
  }
}

static fs_node_t initrd_root;
static fs_node_t root_entries[32];
static uint32_t initrd_start;

//
static uint32_t initrd_read(fs_node_t * node, uint32_t offset, uint32_t size, uint8_t * buffer) {
  if (&node == &initrd_root) {
    memcpy((void *)(initrd_start + offset), (void *) buffer, size);
  }
  return 0;
}

static uint32_t initrd_write(fs_node_t * node, uint32_t offset, uint32_t size, uint8_t * buffer) {
  return 0;
}

static void initrd_open(fs_node_t * node) {
  return;
}

static void initrd_close(fs_node_t * node) {
  return;
}

static struct dirent * initrd_readdir(fs_node_t * node, uint32_t index) {
  return &root_entries[index];
}

static struct fs_node * initrd_finddir(fs_node_t * node, char * name) {
  for (int i = 0; i < 32; i++) {
    if (strcmp(((struct dirent *)initrd_finddir(node, i))->name, name) == 0) {
      return &root_entries[i];
    }
  }
  return NULL;
}

void initrd_load(void * start, void * end, char * cmdline) {
  kprintf("start:   0x%x\nend:    0x%x\n", start, end);
  fs_root = &initrd_root;
  initrd_root.name[0] = '/';
  initrd_root.name[1] = 0;
  initrd_root.read = (read_type_t *)initrd_read;
  initrd_root.write = (read_type_t *)initrd_write;
  initrd_root.open = (read_type_t *)initrd_open;
  initrd_root.close = (read_type_t *)initrd_close;
  initrd_root.readdir = (read_type_t *)initrd_readdir;
  initrd_root.finddir = (read_type_t *)initrd_finddir;

  initrd_start = start;

  cpio_parse(start);
}
