#ifndef FS_H
#define FS_H

#include <stdint.h>

// void * should actually be struct fs_node * 
typedef uint32_t (* read_type_t)(void *, uint32_t, uint32_t, uint8_t *);
typedef uint32_t (* write_type_t)(void *, uint32_t, uint32_t,uint8_t *);
typedef void (* open_type_t)(void *);
typedef void (* close_type_t)(void *);
typedef struct dirent * (* readdir_type_t)(void *, uint32_t);
//typedef struct fs_node * (* finddir_type_t)(struct fs_node *,char * name);

#define FS_FLAGS_FILE 0x01
#define FS_FLAGS_DIRECTORY 0x02

typedef struct fs_node {
  char name[128];
  uint32_t flags;
  uint32_t inode; // implementation specific
  read_type_t read;
  write_type_t write;
  open_type_t open;
  close_type_t close;
  readdir_type_t readdir;
} fs_node_t;

typedef struct dirent // One of these is returned by the readdir call, according to POSIX.
{
  char name[128]; // Filename.
  uint32_t ino;     // Inode number. Required by POSIX.
} dirent_t;

fs_node_t * fs_root;

// Standard read/write/open/close functions. Note that these are all suffixed with
// _fs to distinguish them from the read/write/open/close which deal with file descriptors
// not file nodes.
uint32_t read_fs(fs_node_t * node, uint32_t offset, uint32_t size, uint8_t * buffer);
uint32_t write_fs(fs_node_t * node, uint32_t offset, uint32_t size, uint8_t * buffer);
void open_fs(fs_node_t * node, uint8_t read, uint8_t write);
void close_fs(fs_node_t * node);
dirent_t * readdir_fs(fs_node_t * node, uint32_t index);

#endif
