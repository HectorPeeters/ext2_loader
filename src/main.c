#include <stdio.h>
#include <stdlib.h>

#include "ext2.h"
#include "types.h"

int main(int argc, char **argv)
{
    struct Ext2Fs *fs = ext2_create("disk.img");

    if (ext2_verify(fs))
        printf("EXT2 VALID\n");
    else
        printf("EXT2 FS INVALID\n");

    ext2_print_info(fs);

    struct Inode *root_inode = ext2_read_inode(fs, EXT2_ROOT_INODE);
    ext2_print_inode_info(root_inode);
    struct InodeDirEntry **dir_entries = ext2_read_dir_entries(fs, root_inode->direct_block_pointers[0]);

    uint8 i;
    while (1)
    {
        struct InodeDirEntry *entry = dir_entries[i++];
        if (entry == 0)
            break;

        ext2_print_inode_dir_info(entry);

        if (entry->name[0] != '.')
        {
            struct Inode *in = ext2_read_inode(fs, entry->inode);            

            uint8* data = ext2_read_block_data(fs, in->direct_block_pointers[0], in->lower_bits_size);
            printf("%s\n", data);

            ext2_free(data);
            ext2_free(in);
        }

        ext2_free(entry);
    }

    ext2_free(dir_entries);

    ext2_free(root_inode);

    ext2_destroy(fs);
}