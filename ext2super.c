/*
 * ext2super.c
 *
 * Reads the super-block from a Ext2 floppy disk.
 *
 * Questions?
 * Emanuele Altieri
 * ealtieri@hampshire.edu
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "./ext2_fs.h"

#define BASE_OFFSET 1024                   /* locates beginning of the super block (first group) */
#define FD_DEVICE "./myext2image.img"      /* the floppy disk device */
#define BLOCK_OFFSET(block) (BASE_OFFSET + (block - 1) * block_size)

static unsigned int block_size = 0;        /* block size (to be calculated) */



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

struct ext2_super_block super;

// Function to read super block
void read_super_block(struct ext2_super_block super) {

	printf("Reading super-block from device " FD_DEVICE ":\n"
	       "Inodes count............: %u\n"
	       "Blocks count............: %u\n"
	       "Reserved blocks count...: %u\n"
	       "Free blocks count.......: %u\n"
	       "Free inodes count.......: %u\n"
	       "First data block........: %u\n"
	       "Block size..............: %u\n"
	       "Blocks per group........: %u\n"
	       "Inodes per group........: %u\n"
	       "Creator OS..............: %u\n"
	       "First non-reserved inode: %u\n"
	       "Size of inode structure.: %hu\n"
	       ,
	       super.s_inodes_count,  
	       super.s_blocks_count,
	       super.s_r_blocks_count,     /* reserved blocks count */
	       super.s_free_blocks_count,
	       super.s_free_inodes_count,
	       super.s_first_data_block,
	       block_size,
	       super.s_blocks_per_group,
		   super.s_inodes_per_group
	       );    
		
	printf("\n\n");
}



void read_group_descriptor(struct ext2_group_desc group) {
	printf("Reading first group-descriptor from device " FD_DEVICE ":\n"
	       "Blocks bitmap block: %u\n"
	       "Inodes bitmap block: %u\n"
	       "Inodes table block.: %u\n"
	       "Free blocks count..: %u\n"
	       "Free inodes count..: %u\n"
	       "Directories count..: %u\n"
	       ,
	       group.bg_block_bitmap,
	       group.bg_inode_bitmap,
	       group.bg_inode_table,
	       group.bg_free_blocks_count,
	       group.bg_free_inodes_count,
	       group.bg_used_dirs_count);    /* directories count */

	printf("\n\n");
}

void info() {
	read_super_block();
}



// Function to read inode
static void read_inode(int fd, int inode_no, const struct ext2_group_desc *group, struct ext2_inode *inode)
{
	lseek(fd, BLOCK_OFFSET(group->bg_inode_table) + (inode_no - 1) * sizeof(struct ext2_inode), SEEK_SET);
	read(fd, inode, sizeof(struct ext2_inode));
} /* read_inode() */




// Function to read root inode
void print_read_root_inode(struct ext2_inode inode)
{
	printf("Reading root inode\n"
		   "File mode: %hu\n"
		   "Owner UID: %hu\n"
		   "Size.....: %u bytes\n"
		   "Blocks...: %u\n",
		   inode.i_mode,     /* inode.i_mode */
		   inode.i_uid,      /* inode.i_mode */
		   inode.i_size,     /* inode.i_mode */
		   inode.i_blocks);  /* inode.i_mode */

	for (int i = 0; i < EXT2_N_BLOCKS; i++)
		if (i < EXT2_NDIR_BLOCKS) /* direct blocks */
			printf("Block %2u : %u\n", i, inode.i_block[i]);
		else if (i == EXT2_IND_BLOCK) /* single indirect block */
			printf("Single...: %u\n", inode.i_block[i]);
		else if (i == EXT2_DIND_BLOCK) /* double indirect block */
			printf("Double...: %u\n", inode.i_block[i]);
		else if (i == EXT2_TIND_BLOCK) /* triple indirect block */
			printf("Triple...: %u\n", inode.i_block[i]);

	printf("\n\n");
}


// Function to show entries
static unsigned int read_dir(int fd, const struct ext2_inode *inode, const struct ext2_group_desc *group, char* nomeArquivo)
{
	void *block;

	if (S_ISDIR(inode->i_mode)) {
		struct ext2_dir_entry_2 *entry;
		unsigned int size = 0;

		if ((block = malloc(block_size)) == NULL) { /* allocate memory for the data block */
			fprintf(stderr, "Memory error\n");
			close(fd);
			exit(1);
		}

		lseek(fd, BLOCK_OFFSET(inode->i_block[0]), SEEK_SET);
		read(fd, block, block_size);                /* read block from disk*/

		entry = (struct ext2_dir_entry_2 *) block;  /* first entry in the directory */
                /* Notice that the list may be terminated with a NULL
                   entry (entry->inode == NULL)*/
		while((size < inode->i_size) && entry->inode) {
			char file_name[EXT2_NAME_LEN+1];
			memcpy(file_name, entry->name, entry->name_len);
			file_name[entry->name_len] = 0;     /* append null character to the file name */

			// PARA RETORNAR INODE
			if((strcmp(nomeArquivo, entry->name)) == 0){
				return entry->inode;
			}

			//parametros do cd e ls 
			printf("%s\n"
			       "inode: %10u\n"
				   "Record lenght: %hu\n"
			       "Name lenght: %d\n"
				   "File type: %d\n\n",
					file_name,
					entry->inode,
					entry->rec_len,
					entry->name_len,
					entry->file_type);
			

			entry = (void*) entry + entry->rec_len;
			size += entry->rec_len;
		}

		free(block);
	}

	printf("\n\n");
} /* read_dir() */


unsigned int group_number(unsigned int inode, struct ext2_super_block super) {
	unsigned int group_number = (inode-1) / super.s_inodes_per_group;
	printf("INODE: %hu\n", inode);
	printf("blocos: %hu\n", super.s_inodes_per_group);
	printf("grupo: %hu\n", group_number);
	return group_number;
}




int main(void)
{
	struct ext2_super_block super;
	struct ext2_group_desc group;
	struct ext2_inode inode;
	int fd;
	// int i; (i do for de imprimir read root inode)

	/* open floppy device */

	if ((fd = open(FD_DEVICE, O_RDONLY)) < 0) {
		perror(FD_DEVICE);
		exit(1);  /* error while opening the floppy device */
	}

	
	





	/****** read super-block *******/

	lseek(fd, BASE_OFFSET, SEEK_SET); 
	read(fd, &super, sizeof(super));

	if (super.s_magic != EXT2_SUPER_MAGIC) {
		fprintf(stderr, "Not a Ext2 filesystem\n");
		exit(1);
	}
		
	block_size = 1024 << super.s_log_block_size;

	read_super_block(super);

	/******** TEST INFO **********/
	info();



	/********* read group descriptor ***********/

	lseek(fd, BASE_OFFSET + block_size, SEEK_SET);
	read(fd, &group, sizeof(group));
	//close(fd);
	
	read_group_descriptor(group);




	
	/******** read root inode ********/
 	read_inode(fd, 2, &group, &inode);
 
 	print_read_root_inode(inode);


	close(fd);
	exit(0);
} /* main() */
