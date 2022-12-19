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
#include "time.h"
#include "./ext2_fs.h"


/* Structure of the super block */
/*******************************/
struct ext2_super_block {
	__u32	s_inodes_count;		/* Inodes count */
	__u32	s_blocks_count;		/* Blocks count */
	__u32	s_r_blocks_count;	/* Reserved blocks count */
	__u32	s_free_blocks_count;	/* Free blocks count */
	__u32	s_free_inodes_count;	/* Free inodes count */
	__u32	s_first_data_block;	/* First Data Block */
	__u32	s_log_block_size;	/* Block size */
	__s32	s_log_frag_size;	/* Fragment size */
	__u32	s_blocks_per_group;	/* # Blocks per group */
	__u32	s_frags_per_group;	/* # Fragments per group */
	__u32	s_inodes_per_group;	/* # Inodes per group */
	__u32	s_mtime;		/* Mount time */
	__u32	s_wtime;		/* Write time */
	__u16	s_mnt_count;		/* Mount count */
	__s16	s_max_mnt_count;	/* Maximal mount count */
	__u16	s_magic;		/* Magic signature */
	__u16	s_state;		/* File system state */
	__u16	s_errors;		/* Behaviour when detecting errors */
	__u16	s_minor_rev_level; 	/* minor revision level */
	__u32	s_lastcheck;		/* time of last check */
	__u32	s_checkinterval;	/* max. time between checks */
	__u32	s_creator_os;		/* OS */
	__u32	s_rev_level;		/* Revision level */
	__u16	s_def_resuid;		/* Default uid for reserved blocks */
	__u16	s_def_resgid;		/* Default gid for reserved blocks */

	__u32	s_first_ino; 		/* First non-reserved inode */
	__u16   s_inode_size; 		/* size of inode structure */
	__u16	s_block_group_nr; 	/* block group # of this superblock */
	__u32	s_feature_compat; 	/* compatible feature set */
	__u32	s_feature_incompat; 	/* incompatible feature set */
	__u32	s_feature_ro_compat; 	/* readonly-compatible feature set */
	__u8	s_uuid[16];		/* 128-bit uuid for volume */
	char	s_volume_name[16]; 	/* volume name */
	char	s_last_mounted[64]; 	/* directory where last mounted */
	__u32	s_algorithm_usage_bitmap; /* For compression */
	/*
	 * Performance hints.  Directory preallocation should only
	 * happen if the EXT2_COMPAT_PREALLOC flag is on.
	 */
	__u8	s_prealloc_blocks;	/* Nr of blocks to try to preallocate*/
	__u8	s_prealloc_dir_blocks;	/* Nr to preallocate for dirs */
	__u16	s_padding1;
	__u32	s_reserved[204];	/* Padding to the end of the block */
};

/** DEFINITIONS **/
#define BASE_OFFSET 1024                   /* locates beginning of the super block (first group) */
#define FD_DEVICE "./myext2image.img"      /* the floppy disk device */
#define BLOCK_OFFSET(block) (BASE_OFFSET + (block - 1) * block_size)
#define TAMANHO_ 1000
#define LIMITE_ 4000

/** STACK FUNCTION **/
typedef struct {
    int  tam; // quantos tem
    int  lim; // quantos cabem
    char dado[LIMITE_][TAMANHO_];
} Pilha;


/** AUX FUNCTIONS **/
void read_super_block();
void read_group_descriptor(struct ext2_group_desc group);
void read_inode(int fd, int inode_no, const struct ext2_group_desc *group, struct ext2_inode *inode);
void print_read_root_inode(struct ext2_inode inode);
unsigned int read_dir(int fd, const struct ext2_inode *inode, const struct ext2_group_desc *group, char* nomeArquivo);
//unsigned int group_number(unsigned int inode, struct ext2_super_block super);
void cat(int fd, struct ext2_inode *inode, struct ext2_group_desc *group, char *arquivoNome, int *currentGroup);
void change_group(unsigned int *inode, struct ext2_group_desc *groupToGo, int *currentGroup);
char* catch_principal_param(char* comando);
char* catch_second_param(char* comando);
char* catch_second_param_cp(char* comando);
char* catch_third_param_cp(char* comando);
int verifica_sintaxe(char* segundoPar);


/** COMMANDS **/
void info();
void attr(struct ext2_inode *inode, struct ext2_group_desc *group, char *arquivoNome, int* currentGroup);
void ls(struct ext2_inode*, struct ext2_group_desc*);
void change_directory(char* dirName, struct ext2_inode *inode, struct ext2_group_desc *group, int* currentGroup, Pilha* );
void pwd(Pilha* );
void copia_arquivo(struct ext2_inode* inode, char* originFile, char* destinyFile, struct ext2_group_desc *group, int *currentGroup);