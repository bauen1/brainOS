#ifndef FS_H
#define FS_H

#include <stdint.h>

struct fs_node;
typedef struct fs_node fs_node_t;

// void * should actually be struct fs_node *
typedef uint32_t (* read_type_t)(struct fs_node * node, uint32_t offset, uint32_t size, uint8_t * buffer);
typedef uint32_t (* write_type_t)(struct fs_node * node, uint32_t offset, uint32_t size, uint8_t * buffer);
typedef void (* open_type_t)(struct fs_node * node);
typedef void (* close_type_t)(struct fs_node * node);
typedef struct dirent * (* readdir_type_t)(struct fs_node * node, uint32_t index);
typedef struct fs_node * (* finddir_type_t)(struct fs_node * node, char * name);

#define FS_FLAGS_FILE 0x01
#define FS_FLAGS_DIRECTORY 0x02

typedef struct fs_node {
  char name[128];         // filename
  uint32_t flags;         // flags
  uint32_t inode;         // implementation specific
  read_type_t read;       //
  write_type_t write;     //
  open_type_t open;       //
  close_type_t close;     //
  readdir_type_t readdir; //
  finddir_type_t finddir; //
} fs_node_t;

// One of these is returned by the readdir call, according to POSIX.
typedef struct dirent {
  char name[128]; // Filename.
  uint32_t ino;     // Inode number. Required by POSIX.
} dirent_t;

fs_node_t * fs_root;

uint32_t fs_read(fs_node_t * node, uint32_t offset, uint32_t size, uint8_t * buffer);
uint32_t fs_write(fs_node_t * node, uint32_t offset, uint32_t size, uint8_t * buffer);
void fs_open(fs_node_t * node, uint8_t read, uint8_t write);
void fs_close(fs_node_t * node);
dirent_t * fs_readdir(fs_node_t * node, uint32_t index);
fs_node_t * fs_finddir(fs_node_t * node, char * name);

#endif
