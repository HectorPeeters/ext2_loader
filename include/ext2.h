#pragma once

#include <stdio.h>

#include "types.h"

#define EXT2_SUPERBLOCK_SIZE 1024
#define EXT2_BLOCK_GROUP_DESCRIPTOR_SIZE 32

#define EXT2_ROOT_INODE 2

#define EXT2_STATE_CLEAN 1
#define EXT2_STATE_ERROR 2

#define EXT2_ERROR_IGNORE 1
#define EXT2_ERROR_REMOUNT_READ_ONLY 2
#define EXT2_ERROR_KERNEL_PANIC 3

#define EXT2_OS_LINUX 0
#define EXT2_OS_GNU_HURD 1
#define EXT2_OS_MASIX 2
#define EXT2_OS_FREE_BSD 3
#define EXT2_OS_LITES 4

struct __attribute__((__packed__)) SuperBlock
{
    uint32 inode_count;
    uint32 block_count;
    uint32 super_block_count;

    uint32 unallocated_block_count;
    uint32 unallocated_inode_count;

    uint32 super_block_num;

    uint32 block_size_shift;
    uint32 fragment_size_shift;

    uint32 group_block_count;
    uint32 group_fragment_count;
    uint32 group_inode_count;

    uint32 last_mount_time;
    uint32 last_write_time;

    uint16 mounts_since_consistency_check;
    uint16 max_mounts_before_check;

    // 0xef53
    uint16 signature;
    uint16 state;
    uint16 on_error;
    uint16 minor_version;

    uint32 last_consistency_check;
    uint32 consistency_check_interval;
    uint32 os_id;

    uint32 major_version;

    uint16 reserved_user_id;
    uint16 reserved_group_id;

    // ATTRIBUTES BELOW ARE ONLY PRESENT IF MAJOR VERSION >= 1

    // major version < 1: default = 11
    uint32 first_non_reserved_inode;
    // major version < 1: default = 128
    uint16 inode_struct_size;
    uint16 superblock_group;
    uint32 optional_features;
    uint32 required_features;
    uint32 write_required_features;

    uint8 fs_id[16];
    uint8 volume_name[16];
    uint8 last_path_volume[64];

    uint32 compression;

    uint8 file_prealloc_block_count;
    uint8 dir_prealloc_block_count;

    uint16 __unused;

    // same as fs_id
    uint8 journal_id[16];

    uint32 journal_inode;
    uint32 journal_device;

    uint32 orphan_inode_head;
};

struct __attribute__((__packed__)) BlockGroupDescriptor
{
    uint32 block_usage_block_address;
    uint32 inode_usage_block_address;
    uint32 inode_table_block_address;

    uint16 unalloc_block_count;
    uint16 unalloc_inode_count;
    uint16 dir_count;
};

struct __attribute__((__packed__)) Inode
{
    uint16 type_and_permissions;
    uint16 user_id;

    uint32 lower_bits_size;

    uint32 last_access_time;
    uint32 creation_time;
    uint32 last_modification_time;
    uint32 deletion_time;

    uint16 group_id;

    uint16 hard_link_count;
    uint32 disk_sector_count;

    uint32 flags;
    uint32 os_value;

    uint32 direct_block_pointers[12];

    uint32 singly_indirect_block_pointer;
    uint32 doubly_indirect_block_pointer;
    uint32 triply_indirect_block_pointer;

    uint32 generation_number;

    uint32 extended_attribute_block;
    uint32 upper_bits_size;
    uint32 fragment_block_address;
    uint8 os_value_2[12];
};

struct __attribute__((__packed__)) InodeDirEntry
{
    uint32 inode;
    uint16 size;
    uint8 name_length;
    uint8 type;
    uint8 name[256];
};

struct __attribute__((__packed__)) Ext2Fs
{
    struct SuperBlock *super_block;
    struct BlockGroupDescriptor *block_group_descriptor;
    FILE *file;
};

struct Ext2Fs *ext2_create(const char *path);

void ext2_destroy(struct Ext2Fs *fs);

uint8 ext2_verify(struct Ext2Fs *fs);

void ext2_print_info(struct Ext2Fs *fs);

void ext2_print_inode_info(struct Inode *inode);

uint32 ext2_get_group_count(struct Ext2Fs *fs);

uint32 ext2_get_block_size(struct Ext2Fs *fs);

uint32 ext2_get_fragment_size(struct Ext2Fs *fs);

struct Inode *ext2_read_inode(struct Ext2Fs *fs, uint32 inode_index);
struct InodeDirEntry * ext2_read_dir_entry(struct Ext2Fs *fs, uint32 block);
