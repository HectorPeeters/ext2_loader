#include <stdio.h>
#include <stdlib.h>

#include "ext2.h"
#include "types.h"

int main(int argc, char **argv)
{
    printf("SuperBlock size: %ld\n", sizeof(struct SuperBlock));

    struct Ext2Fs *fs = ext2_create("disk.img");

    if (ext2_verify(fs))
        printf("EXT2 VALID\n");
    else
        printf("EXT2 FS INVALID\n");

    ext2_print_info(fs);

    struct Inode *inode = ext2_read_inode(fs, 2);
    ext2_print_inode_info(inode);
    ext2_read_dir_entry(fs, inode->direct_block_pointers[0]);

    ext2_destroy(fs);

    // printf("\nBlock size: %d\n", ext2_get_block_size(&fs));

    // int inode = EXT2_ROOT_INODE;
    // int block_group = ext2_get_inode_block_group(&fs, inode);
    // printf("\nBlock group: %d\n", block_group);
    // int block_group_index = ext2_get_inode_block_group_index(&fs, inode);
    // printf("Block group index: %d\n", block_group_index);

    // free(buffer);
}