#include "fs.h"

#include <stdint.h>
#include <stddef.h>

uint32_t read_fs(fs_node_t * node, uint32_t offset, uint32_t size, uint8_t * buffer) {
  if (node->read != 0) {
    return node->read(node, offset, size, buffer);
  }
  return 0;
}

uint32_t write_fs(fs_node_t * node, uint32_t offset, uint32_t size, uint8_t * buffer) {
  if (node->write != 0) {
    return node->write(node, offset, size, buffer);
  }
  return 0;
}

void open_fs(fs_node_t * node, uint8_t read, uint8_t write) {
  if (node->open != 0) {
    return node->open(node);
  }
}

void close_fs(fs_node_t * node) {
  if (node->close != 0) {
    return node->close(node);
  }
}

dirent_t * readdir_fs(fs_node_t * node, uint32_t index) {
  if ((node->flags & 0x07) == FS_FLAGS_DIRECTORY && node->readdir != 0) {
    return node->readdir(node, index);
  }
  return NULL;
}
