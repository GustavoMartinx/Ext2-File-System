/** INCLUSIONS **/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>     
#include <sys/wait.h>  
#include <sys/resource.h>
#include "./ext2_fs.h"


/** DEFINITIONS **/
#define BASE_OFFSET 1024                   /* locates beginning of the super block (first group) */
#define FD_DEVICE "./myext2image.img"      /* the floppy disk device */
#define BLOCK_OFFSET(block) (BASE_OFFSET + (block - 1) * block_size)
#define TAMANHO_ 1000
#define LIMITE_ 4000


/** AUX FUNCTIONS **/
void read_super_block();
void read_group_descriptor(struct ext2_group_desc group);
void read_inode(int fd, int inode_no, const struct ext2_group_desc *group, struct ext2_inode *inode);
void print_read_root_inode(struct ext2_inode inode);
unsigned int read_dir(int fd, const struct ext2_inode *inode, const struct ext2_group_desc *group, char* nomeArquivo);
//unsigned int group_number(unsigned int inode, struct ext2_super_block super);
void cat(int fd, struct ext2_inode *inode);
void change_group(unsigned int *inode, struct ext2_group_desc *groupToGo, int *currentGroup);
char* catch_principal_param(char* comando);
char* catch_second_param(char* comando);


/** STACK FUNCTION **/
typedef struct {
    int  tam; // quantos tem
    int  lim; // quantos cabem
    char dado[LIMITE_][TAMANHO_];
} Pilha;


/** COMMANDS **/
void info();
void ls(struct ext2_inode*, struct ext2_group_desc*);
void change_directory(char* dirName, struct ext2_inode *inode, struct ext2_group_desc *group, int* currentGroup /*, Pilha* */);
void pwd(Pilha* );
void touch(int fd, struct ext2_group_desc* group, char* arquivo_nome);