/*https://pt.stackoverflow.com/questions/285713/como-inserir-na-pilha-de-dados-uma-string
 * ext2super.c
 *
 * Reads the super-block from a Ext2 floppy disk.
 *
 * Questions?
 * Emanuele Altieri
 * ealtieri@hampshire.edu
 */

#include "./utils.h"
// #include "stack.c"

static unsigned int block_size = 0;        /* block size (to be calculated) */
struct ext2_super_block super;
int fd;

/******* Pilha **********/

// Insere um elemento/diretório na pilha, retorna o tamanho
int PUSH(const char* val, Pilha* p)
{
    if ((p == NULL) || (val == NULL)) return -1;
    if (strlen(val) > TAMANHO_ - 1) return -2;
    p->tam += 1;
    if (p->tam > p->lim)
    {
        p->tam -= 1;
        return -3;
    }
    strcpy(p->dado[p->tam], val);
    return p->tam;
}

// Diminui o tamanho da pilha: "remover um elemento/diretório"
int POP(Pilha* p)
{
    if (p == NULL) return -1;
    if (p->tam == 0) { //não sei se funciona 
        PUSH("/", p);
        return -2;
    }
    p->tam -= 1;

	// Retorna novo tamanho
    return p->tam;
}

// int TOP(Pilha* p, Registro* val)
// {
//     if ((p == NULL) || (val == NULL)) return -1;
//     if (p->tam == 0) return -2;
//     strcpy(val, p->dado[p->tam]);
//     return 0;
// }

int mostra(Pilha* p, const char* tit)
{
    if (p == NULL)
    {
        //printf("pilha invalida\n");
        return -1;
    }
    if (tit != NULL) printf("%s\n", tit);
    if (p->tam == 0)
    {
        //printf("Pilha VAZIA (cap:%d):\n", p->lim);
        return 0;
    }
    //printf("%d elementos (cap:%d):\n", p->tam, p->lim);
    for (int i = 1; i < p->tam; i += 1)
        printf("/%s", p->dado[i]);
    printf("/%s\n\n", p->dado[p->tam]);
    return 0;
}

Pilha* cria(size_t limite)
{
    Pilha* nova = (Pilha*)malloc(sizeof(Pilha));
    if (nova == NULL) return NULL;
    nova->lim = (int) limite;
    nova->tam = 0; // marca como vazia
    return nova; // tudo certo: retorna a pilha nova
}

Pilha* destroi(Pilha* pilha)
{
    if (pilha == NULL) return NULL;
    free(pilha->dado);
    free(pilha);
    return NULL;
}


// Function to read super block
void read_super_block() {

	printf(
		   "Volume name.............: %s\n"
		   "Image size..............: %u bytes\n" 
		   "Free space..............: %u KiB\n" 
	       "Free inodes ............: %u\n"
		   "Free blocks ............: %u\n"
		   "Block size..............: %u bytes\n"
		   "Inode size..............: %hu bytes\n"
		   "Groups count............: %u\n"
	       "Groups size.............: %u blocks\n"
	       "Groups inodes...........: %u inodes\n"
		   "Inodetable size.........: %lu blocks\n"
	       ,  
		   super.s_volume_name, //nome do volume 
		   (super.s_blocks_count * block_size /* super.s_blocks_per_group*/), //tamanho da imagem
		   (super.s_free_blocks_count * block_size) / 1024, // espaço livre //bug???
	       super.s_free_inodes_count, //free inodes 
		   super.s_free_blocks_count, //free blocks
		   block_size, // tamanho do bloco
		   super.s_inode_size, //inode size 
		   (super.s_blocks_count/super.s_blocks_per_group), //bug??
		   super.s_blocks_per_group,
		   super.s_inodes_per_group,
		   (super.s_inodes_per_group/(block_size/sizeof(struct ext2_inode)))
	       );
		
}


// Function to read group descriptor
void read_group_descriptor(struct ext2_group_desc group) {
	printf("Reading first group-descriptor from device " FD_DEVICE ":\n"
	       "Blocks bitmap block: %u\n"
	       "Inodes bitmap block: %u\n"
	       "Inodes table block.: %u\n"
	       "Free blocks count..: %u\n"
	       "Free inodes count..: %u\n"
	       "Directories count..: %u\n",
	       group.bg_block_bitmap,
	       group.bg_inode_bitmap,
	       group.bg_inode_table,
	       group.bg_free_blocks_count,
	       group.bg_free_inodes_count,
	       group.bg_used_dirs_count);    

	printf("\n\n");
}

void info() {
	read_super_block();
}



// Function to read inode
void read_inode(int fd, int inode_no, const struct ext2_group_desc *group, struct ext2_inode *inode)
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
		   inode.i_mode,   
		   inode.i_uid,    
		   inode.i_size,   
		   inode.i_blocks);

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
unsigned int read_dir(int fd, const struct ext2_inode *inode, const struct ext2_group_desc *group, char* nomeArquivo)
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
		read(fd, block, block_size);                /* read block from disk */

		entry = (struct ext2_dir_entry_2 *) block;  /* first entry in the directory */

		while((size < inode->i_size) && entry->inode) {

			char file_name[EXT2_NAME_LEN+1];
			memcpy(file_name, entry->name, entry->name_len);
			file_name[entry->name_len] = 0;     	/* append null character to the file name */

			// PARA RETORNAR INODE
			if((strcmp(nomeArquivo, entry->name)) == 0){
				return entry->inode;
			}
			
			// Iteration
			entry = (void*) entry + entry->rec_len;  // casting
			size += entry->rec_len;
		}

		free(block);
	}

	printf("\n\n");
} /* read_dir() */


// Function to get the number of a group
unsigned int group_number(unsigned int inode, struct ext2_super_block super) {
	unsigned int group_number = (inode-1) / super.s_inodes_per_group;
	return group_number;
}


// Function to print the content of a () file
void cat(int fd, struct ext2_inode *inode)
{
	int size = inode->i_size;
	char *block = malloc(block_size);

	lseek(fd, BLOCK_OFFSET(inode->i_block[0]), SEEK_SET);
	read(fd, block, block_size);

	for(int i = 0; i < size; i++){
		printf("%c",block[i]);
	}
}

void ls(struct ext2_inode *inode, struct ext2_group_desc *group) {

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

		while((size < inode->i_size) && entry->inode) {

			char file_name[EXT2_NAME_LEN+1];
			memcpy(file_name, entry->name, entry->name_len);
			file_name[entry->name_len] = 0;     	/* append null character to the file name */
 
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
			
			// Iteration
			entry = (void*) entry + entry->rec_len;  // casting
			size += entry->rec_len;
		}

		free(block);
	}

	printf("\n\n");
}

void attr(struct ext2_inode *inode, struct ext2_group_desc *group, char *arquivoNome, int* currentGroup){
	struct ext2_inode* entry = (struct ext2_inode*)malloc(sizeof(struct ext2_inode));
	struct ext2_group_desc *grupoTemp = (struct ext2_group_desc *)malloc(sizeof(struct ext2_group_desc));
	memcpy(entry, inode, sizeof(struct ext2_inode));
	memcpy(grupoTemp, group, sizeof(struct ext2_group_desc));
	unsigned int novo_inode = read_dir(fd, inode, group, arquivoNome);
	change_group(&novo_inode, grupoTemp, currentGroup);
	read_inode(fd, novo_inode, grupoTemp, entry);
	/*printf("%s\n"
			"permissões %hu\t"
			"uid %hu\t"
			"gid %hu\t"
			"tamanho %u\t",
			//file_name,
			entry->acle_size,
			entry->acle_perms,
			entry->acle_tag,
			entry->acle_tag
			);*/
	printf(
			"permissões %hu\t"
			"uid %hu\t"
			"gid %hu\t"
			"tamanho %hu\t"
			"modificado em %d\t",
			entry->i_mode,
			entry->i_uid,
			entry->i_gid,
			entry->i_size,
			entry->i_mtime
			);
}

void pwd(Pilha* stack) {
	//Der exit apagar toda a stack
	//mostra(&stack);
}

void change_directory(char* dirName, struct ext2_inode *inode, struct ext2_group_desc *group, int *currentGroup /*, Pilha* stack*/) {

	/*
	struct ext2_dir_entry_2 *entry;
	procura o nome do arquivo correspondente, se encontrar coloca na pilha
	fazer uma inicialização da pilha contendo o nome do diretótio principal???
	if(nomeArquivo != ".." && nomeArquivo != "."){
		PUSH(nomeArquivo, &stack);
	}
	else if(nomeArquivo == ".."){
		POP(&stack);
	} 
	*/

/*cd livros
livros
inode: 8193
record lenght: 16
name lenght: 6
file type: 2*/


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

		while((size < inode->i_size) && entry->inode) {
			char file_name[EXT2_NAME_LEN+1];
			memcpy(file_name, entry->name, entry->name_len);
			file_name[entry->name_len] = 0;     /* append null character to the file name */

			// PARA RETORNAR INODE
			if((strcmp(dirName, entry->name)) == 0){
				if(entry->name != ".." && entry->name != "."){
					//PUSH(entry->name, &stack);
				}
				else if(entry->name == ".."){
					//POP(&stack);
				}
				
				//parametros do cd 
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
				break;
			}

			entry = (void*) entry + entry->rec_len;
			size += entry->rec_len;
		}

		free(block);
	}
	
	printf("\n\n");



	read_inode(fd, 2, group, inode);

	unsigned int dirNameInode = read_dir(fd, inode, group, dirName);

	change_group(&dirNameInode, group, currentGroup);

	unsigned int index = ((int)dirNameInode) % super.s_inodes_per_group;

	read_inode(fd, index, group, inode);

	// apenas para teste
	ls(inode, group);
	
	
}





void change_group(unsigned int* inode, struct ext2_group_desc* groupToGo, int* currentGroup) {

	unsigned int block_group = ((*inode) - 1) / super.s_inodes_per_group; // Cálculo do grupo do Inode

	if (block_group != (*currentGroup))
	{
		*currentGroup = block_group;

		lseek(fd, BASE_OFFSET + block_size + sizeof(struct ext2_group_desc) * block_group, SEEK_SET);
		read(fd, groupToGo, sizeof(struct ext2_group_desc));
	}
}

// Separa segundo comando do input completo
char* catch_second_param(char* comando) {

	if(strchr(comando, ' ') != NULL) {

		char* forward_space_position = calloc(50, sizeof(char));
		forward_space_position = strchr(comando, ' ');  // retorna o restante da string a partir de onde estiver o char ' ' (espaço).

		// printf("space position content(com spaco):%s\n", forward_space_position); // conteúdo do segundo parâmetro com um espaço na primeira posição

		// To remove the space:
		// moving each position backward
		for(int j = 0; forward_space_position[j+1] != '\0'; j++) {
			forward_space_position[j] = forward_space_position[j+1];
		}
		
		// putting '\0' in last position of forward_space_position
		int second_param_len = strlen(forward_space_position);
		forward_space_position[second_param_len - 1] = '\0';

		return forward_space_position;
	}
}

// Separa primeiro/principal comando do input completo
char* catch_principal_param(char* comando) {
	
	char* buff = calloc(50, sizeof(char));

	for(int i = 0; comando[i] != ' '; i++) {
			
		buff[i] = comando[i];
	}

	return buff;
}




//saber em que grupo estou 
//olhar se há espaço disponivel 
//se sim onde alucar o arquivo 
//criar o arquivo
//colocar o nome passado 
//marcar no map de bits 
//marcar no mapa de inode 
//tirar do espaço dsponível 
void touch(int fd, struct ext2_group_desc* group, char* arquivo_nome){
	group->bg_block_bitmap;		/* Blocks bitmap block */
	group->bg_inode_bitmap;		/* Inodes bitmap block */
	group->bg_inode_table;		/* Inodes table block */
}





int main() {

	struct ext2_inode inode;
	struct ext2_group_desc group;
	int currentGroup = 0;
	
	/*Create stack */
	// Pilha stack = {.tam = 0, .lim = TAMANHO_};
	

 	/* open floppy device */
 	if ((fd = open(FD_DEVICE, O_RDONLY)) < 0) {
 		perror(FD_DEVICE);
 		exit(1);  /* error while opening the floppy device */
 	}



// 	/****** read super-block *******/
// 	/******************************/

 	lseek(fd, BASE_OFFSET, SEEK_SET); 
 	read(fd, &super, sizeof(super));

 	if (super.s_magic != EXT2_SUPER_MAGIC) {
 		fprintf(stderr, "Not a Ext2 filesystem\n");
 		exit(1);
 	}
		
 	block_size = 1024 << super.s_log_block_size;
 	//read_super_block(super);



// 	/******** TEST INFO **********/
// 	info();



// 	/********* read group descriptor ***********/
// 	/******************************************/
	lseek(fd, BASE_OFFSET + block_size, SEEK_SET);
 	read(fd, &group, sizeof(group));
	
// 	read_group_descriptor(group);




	
 	/******** read root inode ********/
 	/********************************/
	// read_inode(fd, 2, &group, &inode);
 
  	//print_read_root_inode(inode);



	/* show entries in the root directory */
	/*************************************/
	read_inode(fd, 2, &group, &inode);  // read inode 2 (root directory)
	read_dir(fd, &inode, &group, "/");

	
	
	/******** TEST LS **********/
//	printf("***** TEST LS ******\n\n");
	//// ls(&inode, &group); ////


 	/******** PRINT FILE CONTENT **********/
 	// read_inode(fd, 12, &group, &inode);
 	// cat(fd, &group, &inode);



	
	/******** TEST CAT (p/ arq especifico -> não. precisamos do cd) **********/
//	read_inode(fd, 2, &group, &inode);
//	unsigned int entry_inode = read_dir(fd, &inode, &group, "hello.txt");

//	read_inode(fd, entry_inode, &group, &inode);
//	cat(fd, &inode);


//	/************* TEST GETTING GROUP NUMBER *******************/
//	entry_inode = 12289;  // documentos
//	unsigned int gp_number = group_number(entry_inode, super);
//	printf("%hu\n", gp_number);


//	/************ TEST ATTR **************/
//	read_inode(fd, 2, &group, &inode);
//	unsigned int entry_inod = read_dir(fd, &inode, &group, "hello.txt");


 	/********* TEST CHANGE GROUP ********/
 	// changeGroup(&inode, &groupToGo, &currentGroup);
	

 	/******* TEST CD *****/
// 	printf("*************************************************\n");
// 	change_directory("livros", &inode, &group, &currentGroup /*, &stack */);

// 	change_directory("religiosos", &inode, &group, &currentGroup, &stack);


	/********** TESTE: TUDO NO SHELL ********/
	char *fullCommand = calloc(50, sizeof(char));
	
	while (1)
    {
        printf("shell$ ");

		fgets(fullCommand, 50, stdin);  // Captura comando completo pelo shell. Ex.: cat fileName
    	fullCommand[strcspn(fullCommand, "\n")] = 0;

		// Alocações para comando principal e seu parâmetro (se houver)
		char* comando = calloc(50, sizeof(char));
		char* second_param = calloc(50, sizeof(char));
		
		if(strchr(fullCommand, ' ') != NULL) {
			
			// Captura comando principal (comando) 		  Ex.: cat
			comando = catch_principal_param(fullCommand);
			// printf("\ncomando principal: %s\n", comando);

			// Captura segundo comando (second_param)     Ex.: fileName
			second_param = catch_second_param(fullCommand);
			// printf("\n segundo param la fora: %s\n", second_param);

		} else if(strchr(fullCommand, ' ') == NULL) {
			
			// Captura comandos compostos por apenas uma palavra. Ex.: info
			strcpy(comando, fullCommand);
		}
		

		/****** Comparações: entrada == comando esperado  ******/

        if((strcmp(comando, "ls")) == 0) {
           printf("***** TEST LS ******\n\n");
	       ls(&inode, &group);
        }

        else if((strcmp(comando, "info")) == 0) {
            printf("teste");
            info();
        }

        else if((strcmp(comando, "cat")) == 0) {

			read_inode(fd, 2, &group, &inode);
			unsigned int entry_inode = read_dir(fd, &inode, &group, second_param);
			read_inode(fd, entry_inode, &group, &inode);
            cat(fd,&inode);
        }

        else if((strcmp(comando, "attr")) == 0) {
            // chama attr;
    	}

	}




	close(fd);
	exit(0);
}
