#include "ext2.h"

#include <stdio.h>
#include <stdlib.h>

struct Ext2Fs *ext2_create(const char *path)
{
    struct Ext2Fs *fs = malloc(sizeof(struct Ext2Fs));

    fs->file = fopen(path, "rb");

    fs->super_block = malloc(sizeof(struct SuperBlock));
    fseek(fs->file, 1024, SEEK_SET);
    fread(fs->super_block, sizeof(struct SuperBlock), 1, fs->file);

    fs->block_group_descriptor = malloc(sizeof(struct BlockGroupDescriptor));
    fseek(fs->file, 1024 + ext2_get_block_size(fs), SEEK_SET);
    fread(fs->block_group_descriptor, sizeof(struct BlockGroupDescriptor), 1, fs->file);

    return fs;
}

void ext2_destroy(struct Ext2Fs *fs)
{
    fclose(fs->file);

    free(fs->super_block);
    free(fs->block_group_descriptor);
    free(fs);
}

uint8 ext2_verify(struct Ext2Fs *fs)
{
    // check signature
    if (fs->super_block->signature != 0xef53)
        return 0;

    // check group counts (rounding up)
    uint32 block_group_count = (fs->super_block->block_count + (fs->super_block->group_block_count - 1)) / fs->super_block->group_block_count;
    uint32 inode_group_count = (fs->super_block->inode_count + (fs->super_block->group_inode_count - 1)) / fs->super_block->group_inode_count;

    if (block_group_count != inode_group_count)
        return 0;

    return 1;
}

void ext2_print_info(struct Ext2Fs *fs)
{
    struct SuperBlock *sb = fs->super_block;

    printf("SuperBlock\n");
    printf("\tInode count: %d\n", sb->inode_count);
    printf("\tBlock count: %d\n", sb->block_count);
    printf("\tSuper user block count: %d\n", sb->super_block_count);
    printf("\n");
    printf("\tUnallocated block count: %d\n", sb->unallocated_block_count);
    printf("\tUnallocated inode count: %d\n", sb->unallocated_inode_count);
    printf("\n");
    printf("\tSuper block number: %d\n", sb->super_block_num);
    printf("\n");
    printf("\tBlock size shift: %d\n", sb->block_size_shift);
    printf("\tFragment size shift: %d\n", sb->fragment_size_shift);
    printf("\n");
    printf("\tGroup block count: %d\n", sb->group_block_count);
    printf("\tGroup fragment count: %d\n", sb->group_fragment_count);
    printf("\tGroup inode count: %d\n", sb->group_inode_count);
    printf("\n");
    printf("\tLast mount time: %d\n", sb->last_mount_time);
    printf("\tLast write time: %d\n", sb->last_write_time);
    printf("\tMounts since last consitency check: %d\n", sb->mounts_since_consistency_check);
    printf("\tMax mounts before consistency check: %d\n", sb->max_mounts_before_check);
    printf("\n");
    printf("\tSignature: %x\n", sb->signature);
    printf("\tState: %x\n", sb->state);
    printf("\tOn error: %x\n", sb->on_error);
    printf("\tMinor version: %x\n", sb->minor_version);
    printf("\n");
    printf("\tLast consistency check: %d\n", sb->last_consistency_check);
    printf("\tConsistency check interval: %d\n", sb->consistency_check_interval);
    printf("\tOS id: %d\n", sb->os_id);
    printf("\n");
    printf("\tMajor version: %d\n", sb->major_version);
    printf("\n");
    printf("\tReserved user id: %d\n", sb->reserved_user_id);
    printf("\tReserved group id: %d\n", sb->reserved_group_id);
    printf("\n");

    if (sb->major_version > 1)
    {
        // TODO: implement the rest of this printing method
    }

    struct BlockGroupDescriptor *bgd = fs->block_group_descriptor;

    printf("Block Group Descriptor\n");
    printf("\tBlock address of block usage bitmap: %d\n", bgd->block_usage_block_address);
    printf("\tBlock address of inode usage bitmap: %d\n", bgd->inode_usage_block_address);
    printf("\tBlock address of inode table: %d\n", bgd->inode_table_block_address);
    printf("\n");
    printf("\tUnallocated block count: %d\n", bgd->unalloc_block_count);
    printf("\tUnallocated inode count: %d\n", bgd->unalloc_inode_count);
    printf("\tDirectory count: %d\n\n", bgd->dir_count);
}

void ext2_print_inode_info(struct Inode *inode)
{
    printf("Inode\n");
    printf("\tType and permissions: %x\n", inode->type_and_permissions);
    printf("\tUser id: %d\n", inode->user_id);
    printf("\tLower bits size: %d\n", inode->lower_bits_size);
    printf("\n");
    printf("\tLast access time: %d\n", inode->last_access_time);
    printf("\tCreation time: %d\n", inode->creation_time);
    printf("\tLast modification time: %d\n", inode->last_modification_time);
    printf("\tDeletion time: %d\n", inode->deletion_time);
    printf("\n");
    printf("\tGroup id: %d\n", inode->group_id);
    printf("\n");
    printf("\tHard link count: %d\n", inode->hard_link_count);
    printf("\tDisc sector count: %d\n", inode->disk_sector_count);
    printf("\n");
    printf("\tFlags: %x\n", inode->flags);
    printf("\tOS value: %d\n", inode->os_value);
    printf("\n");
    for (int i = 0; i < 12; i++)
        printf("\tDirect block pointer %d: %d\n", i, inode->direct_block_pointers[i]);
    printf("\n");
    printf("\tSingly indirect block pointer: %d\n", inode->singly_indirect_block_pointer);
    printf("\tDoubly indirect block pointer: %d\n", inode->doubly_indirect_block_pointer);
    printf("\tTriply indirect block pointer: %d\n", inode->triply_indirect_block_pointer);
    printf("\n");
    printf("\tGeneration number: %d\n", inode->generation_number);
    printf("\tExtended attribute block: %d\n", inode->extended_attribute_block);
    printf("\tFragment block address: %d\n", inode->fragment_block_address);
    printf("\tOS value 2: ");
    for (int i = 0; i < 12; i++)
        printf("%d ", inode->os_value_2[i]);
    printf("\n\n");
}

uint32 ext2_get_group_count(struct Ext2Fs *fs)
{
    return (fs->super_block->block_count + (fs->super_block->group_block_count - 1)) / fs->super_block->group_block_count;
}

uint32 ext2_get_block_size(struct Ext2Fs *fs)
{
    return 1024 << fs->super_block->block_size_shift;
}

uint32 ext2_get_fragment_size(struct Ext2Fs *fs)
{
    return 1024 << fs->super_block->fragment_size_shift;
}

struct Inode *ext2_read_inode(struct Ext2Fs *fs, uint32 inode_index)
{
    uint32 group_number = (inode_index - 1) / fs->super_block->group_inode_count;
    uint32 group_index = (inode_index - 1) % fs->super_block->group_inode_count;
    uint32 inode_table_block_number = fs->block_group_descriptor->inode_table_block_address;

    fseek(fs->file, inode_table_block_number * ext2_get_block_size(fs) + group_index * sizeof(struct Inode), SEEK_SET);

    struct Inode *inode = (struct Inode *)malloc(sizeof(struct Inode));
    fread(inode, sizeof(struct Inode), 1, fs->file);

    return inode;
}

struct InodeDirEntry *ext2_read_dir_entry(struct Ext2Fs *fs, uint32 block)
{
    uint32 block_size = ext2_get_block_size(fs);

    uint32 total_size = 0;

    struct InodeDirEntry *entry = (struct InodeDirEntry *)malloc(sizeof(struct InodeDirEntry));

    fseek(fs->file, block * block_size, SEEK_SET);

    while (total_size < block_size)
    {
        fread(entry, sizeof(struct InodeDirEntry), 1, fs->file);
        printf("%s, size: ", entry->name);
        printf("%d\n", entry->size);
        total_size += entry->size;
        fseek(fs->file, -(sizeof(struct InodeDirEntry) - entry->size), SEEK_CUR);
    }
}