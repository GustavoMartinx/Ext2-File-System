/** INCLUSIONS **/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "./ext2_fs.h"

/** DEFINITIONS **/
#define BASE_OFFSET 1024                   /* locates beginning of the super block (first group) */
#define FD_DEVICE "./myext2image.img"      /* the floppy disk device */
#define BLOCK_OFFSET(block) (BASE_OFFSET + (block - 1) * block_size)

/** AUX FUNCTIONS **/
void read_super_block();
void read_group_descriptor(struct ext2_group_desc group);
static void read_inode(int fd, int inode_no, const struct ext2_group_desc *group, struct ext2_inode *inode);
void print_read_root_inode(struct ext2_inode inode);
static unsigned int read_dir(int fd, const struct ext2_inode *inode, const struct ext2_group_desc *group, char* nomeArquivo);
//unsigned int group_number(unsigned int inode, struct ext2_super_block super);
void read_file(int fd, struct ext2_inode *inode);
void chageGroup(unsigned int *inode, struct ext2_group_desc *groupToGo, int *currentGroup);

/** COMMANDS **/
void info();
void ls();
void attr();
void change_directory(char* dirName, struct ext2_inode *inode, struct ext2_group_desc *group, int *currentGroup);
void pwd();