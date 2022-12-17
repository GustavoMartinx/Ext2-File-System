//Autores: AmandaFerrari, Catarine Cruz e Gustavo Zanzin.

#include "./utils.h"

static unsigned int block_size = 0;        /* block size (to be calculated) */
struct ext2_super_block super;
int fd;

/*Alterar depois???*/
#define EXT2_S_IRUSR 0x0100 // user read
#define EXT2_S_IWUSR 0x0080 // user write
#define EXT2_S_IXUSR 0x0040 // user execute
#define EXT2_S_IRGRP 0x0020 // group read
#define EXT2_S_IWGRP 0x0010 // group write
#define EXT2_S_IXGRP 0x0008 // group execute
#define EXT2_S_IROTH 0x0004 // others read
#define EXT2_S_IWOTH 0x0002 // others write
#define EXT2_S_IXOTH 0x0001 // others execute


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
    if (p->tam == 0) {
        // PUSH("/", p);
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
		printf("/");
        return 0;
    }
    //printf("%d elementos (cap:%d):\n", p->tam, p->lim);
    for (int i = 1; i < p->tam; i += 1)
        printf("/%s", p->dado[i]);
    printf("/%s", p->dado[p->tam]);
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
		   ((super.s_free_blocks_count - super.s_r_blocks_count) * block_size) / 1024, // espaço livre //bug???
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
		// printf("QUERO SABER SE ALFABETO ENTRA AQUI");
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
			if((strcmp(nomeArquivo, file_name)) == 0) {
				return entry->inode;
			}
			
			// Iteration
			entry = (void*) entry + entry->rec_len;
			size += entry->rec_len;
		}

		free(block);
	}

	printf("\n\n");
	
	//********TESTE *********
	// return 1;
} 


// Function to get the number of a group
unsigned int group_number(unsigned int inode, struct ext2_super_block super) {
	unsigned int group_number = (inode-1) / super.s_inodes_per_group;
	return group_number;
}


// Function to print the content of a file
void cat(int fd, struct ext2_inode *inode, struct ext2_group_desc *group, char *arquivoNome, int *currentGroup)
{	
	struct ext2_group_desc *grupoTemp = (struct ext2_group_desc *)malloc(sizeof(struct ext2_group_desc));
	struct ext2_inode* inodeEntryTemp = (struct ext2_inode*)malloc(sizeof(struct ext2_inode));
	
	memcpy(grupoTemp, group, sizeof(struct ext2_group_desc));
	memcpy(inodeEntryTemp, inode, sizeof(struct ext2_inode));
	unsigned int inodeRetorno = read_dir(fd, inodeEntryTemp, grupoTemp, arquivoNome);
	
	printf("\ninodeRetorno: %u\n", inodeRetorno);

	change_group(&inodeRetorno, grupoTemp, currentGroup);

	unsigned int index = inodeRetorno % super.s_inodes_per_group;
	
	read_inode(fd, index, grupoTemp, inodeEntryTemp);
	

	char *block = (char*)malloc(sizeof(char)*block_size);

	lseek(fd, BLOCK_OFFSET(inodeEntryTemp->i_block[0]), SEEK_SET);
	read(fd, block, block_size);

	int arqSize = inodeEntryTemp->i_size;
	
	int singleIndirection[256];
	int doubleIndirection[256];

	// printf("ARQ SIZE: %d ", arqSize);

	// Percorrendo pelos blocos de dados sem indireção
	for(int i = 0; i < 12; i++) {

		lseek(fd, BLOCK_OFFSET(inodeEntryTemp->i_block[i]), SEEK_SET);
		read(fd, block, block_size); // Lê bloco i em block
		
		// Exibindo conteúdo do primeiro bloco
		for(int i = 0; i < 1024; i++) {
			printf("%c", block[i]);
			
			arqSize = arqSize - sizeof(char); // Quantidade de dados restantes

			if(arqSize <= 0) {
				break;
			}
		}
		if(arqSize <= 0) {
			break;
		}
	}

	// Se após os blocos sem indireção ainda existirem dados,
	// percorre o bloco 12 (uma indireção)
	if(arqSize > 0) {
		
		lseek(fd, BLOCK_OFFSET(inodeEntryTemp->i_block[12]), SEEK_SET);
		read(fd, singleIndirection, block_size);
		
		for(int i = 0; i < 256; i++) {
			
			lseek(fd, BLOCK_OFFSET(singleIndirection[i]), SEEK_SET);
			read(fd, block, block_size);

			for(int j = 0; j < 1024; j++) {
				
				printf("%c", block[j]);
				arqSize = arqSize - 1;
				
				if (arqSize <= 0) {
					break;
				}
			}
			if (arqSize <= 0) {
				break;
			}
		}
	}

	// Se depois dos blocos com uma indireção ainda existirem dados,
	// percorre o bloco 13 (dupla indireção)
	if(arqSize > 0){

		lseek(fd, BLOCK_OFFSET(inodeEntryTemp->i_block[13]), SEEK_SET);
		read(fd, doubleIndirection, block_size);

		for(int i = 0; i < 256; i++){
			
			//não entendi
			if(arqSize <= 0){
				break;
			}

			lseek(fd, BLOCK_OFFSET(doubleIndirection[i]), SEEK_SET);
			read(fd, singleIndirection, block_size);

			for(int j = 0; j < 256; j++) {
				
				if (arqSize <= 0){
					break;
				}

				lseek(fd, BLOCK_OFFSET(singleIndirection[j]), SEEK_SET);
				read(fd, block, block_size);

				for(int k = 0; k < 1024; k++){
					
					printf("%c", block[k]);
					
					arqSize = arqSize - 1;
					
					if (arqSize <= 0){
						break;
					}
				}
			}
		}
	}

	// Fazendo o inode voltar a ser o inode do diretório em que estava no momento em que
	// o cat de um arquivo foi chamado.
	unsigned int current_dir_entry_inode = read_dir(fd, inodeEntryTemp, grupoTemp, ".");

	read_inode(fd, current_dir_entry_inode, grupoTemp, inodeEntryTemp);

	// Desalocando memória
	free(block);
	free(grupoTemp);
	free(inodeEntryTemp);

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
	
	unsigned int inodeRetorno = read_dir(fd, entry, grupoTemp, arquivoNome);
	
	change_group(&inodeRetorno, grupoTemp, currentGroup);
	
	unsigned int index = inodeRetorno % super.s_inodes_per_group;
	
	read_inode(fd, index, grupoTemp, entry);
	
	/*verificar se é um arquivo ou um diretorio*/
	char fileOrDir;
	if(S_ISDIR(entry->i_mode)){
		fileOrDir = 'd';
	}else {
		fileOrDir = 'f';
	}

	/*verificar as permisões do usuario*/
	char uRead; 
	char uWrite;
	char uExec;
	if((entry->i_mode) & (EXT2_S_IRUSR)){
		uRead = 'r';
	} else{
		uRead = '-';
	}
	if ((entry->i_mode) & (EXT2_S_IWUSR)){
		uWrite = 'w';
	}else{
		uWrite = '-';
	}
	if ((entry->i_mode) & (EXT2_S_IXUSR)){
		uExec = 'x';
	}else{
		uExec = '-';
	}

	/*verificar as permisões do grupo*/
	char gRead; 
	char gWrite;
	char gExec;

	if ((entry->i_mode) & (EXT2_S_IRGRP)){
		gRead = 'r';
	}else{
		gRead = '-';
	}
	if ((entry->i_mode) & (EXT2_S_IWGRP)){
		gWrite = 'w';
	}else{
		gWrite = '-';
	}
	if ((entry->i_mode) & (EXT2_S_IXGRP)){
		gExec = 'x';
	}else{
		gExec = '-';
	}

	/*verificar as permisões do grupo*/
	char oRead; 
	char oWrite;
	char oExec;
	if ((entry->i_mode) & (EXT2_S_IROTH)){
		oRead = 'r';
	}else{
		oRead = '-';
	}
	if ((entry->i_mode) & (EXT2_S_IWOTH))
		oWrite = 'w';
	else
		oWrite = '-';
	if ((entry->i_mode) & (EXT2_S_IXOTH))
		oExec = 'x';
	else
		oExec = '-';
	
	printf(
			"permissões\t"
			"uid \t"
			"gid \t"
			"tamanho \t"
			"modificado em\t\n"
			//"%hu\t\t"
			"%c"
			"%c"
			"%c"
			"%c"
			"%c"
			"%c"
			"%c"
			"%c"
			"%c"
			"%c\t"
			"%d\t"
			"%d ",
			//"%hu\t\t"
			//"%d\t\t\n",
			fileOrDir,
			uRead,
			uWrite,
			uExec,
			gRead,
			gWrite,
			gExec,
			oRead,
			oWrite,
			oExec,
			entry->i_uid,
			entry->i_gid
			//entry->i_size,
			//entry->i_mtime
			);
		/*Colocar a unidade correta*/
	if (entry->i_size > 1024){
		printf("   %.1f KiB", (((float)entry->i_size) / 1024));
	}else{
		printf("    %d B ", (entry->i_size));
	}
}

void pwd(Pilha* stack) {
	//Der exit apagar toda a stack
	mostra(stack, NULL);
	printf("\n");
}

void change_directory(char* dirName, struct ext2_inode *inode, struct ext2_group_desc *group, int *currentGroup, Pilha* stack) {

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
			if((strcmp(dirName, file_name)) == 0){
				// printf("entry->name: [%s]\n", entry->name);
				if((strcmp(entry->name, "..") != 0) && (strcmp(entry->name, ".") != 0)){
					PUSH(entry->name, stack);
				}
				else if(strcmp(entry->name, "..") == 0){
					POP(stack);
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

	unsigned int inodeRetorno = read_dir(fd, inode, group, dirName);

	change_group(&inodeRetorno, group, currentGroup);

	unsigned int index = ((int)inodeRetorno) % super.s_inodes_per_group;

	read_inode(fd, index, group, inode);
	
	
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

		char* forward_space_position = calloc(100, sizeof(char));
		forward_space_position = strchr(comando, ' ');  // retorna o restante da string a partir de onde estiver o char ' ' (espaço).

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
	
	char* buff = calloc(100, sizeof(char));

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
	 Pilha stack = {.tam = 0, .lim = TAMANHO_};
	

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
	char* diretorio = calloc(50, sizeof(char));
	char *fullCommand = calloc(100, sizeof(char));
	while (1)
    {

        //printf("[/%s]$> ",diretorio);
		printf("[");
		mostra(&stack, NULL);
		printf("]$> ");

		fgets(fullCommand, 100, stdin);  // Captura comando completo pelo shell. Ex.: cat fileName
    	fullCommand[strcspn(fullCommand, "\n")] = 0;

		// Alocações para comando principal e seu parâmetro (se houver)
		char* comando = calloc(100, sizeof(char));
		char* second_param = calloc(100, sizeof(char));
		second_param = "NULL";
		
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
	       ls(&inode, &group);
        }

        else if((strcmp(comando, "info")) == 0) {
            info();
        }

        else if((strcmp(comando, "cat")) == 0) {
			// verificação se o arquivo solicitado existe naquele dir
			// pré-processamento de onde estará inode/group daquele arquivo (procurar apenas no dir em que estamos naquele momento)
			//read_inode(fd, 2, &group, &inode);
			// unsigned int entry_inode = read_dir(fd, &inode, &group, second_param); NÃO DESCOMENTAR

			// ********* TESTE TRATAMENTO DE ERROS **********
			// if(strcmp(second_param, "NULL") == 0){
			// 	printf("invalid sintax.\n");
			// }


			// if(entry_inode == 1 && (strcmp(second_param, "NULL") != 0)){
			// 	printf("directory not found.\n");
			// }
			// ********* TESTE **********
			
			// read_inode(fd, entry_inode, &group, &inode); NÃO DESCOMENTAR
			
            cat(fd, &inode, &group, second_param, &currentGroup);

			// retorna o inode para o root (diretorio em que estava no momento em que o cat foi chamado)
			// read_inode(fd, 2, &group, &inode);
        }

        else if((strcmp(comando, "attr")) == 0) {
            attr(&inode, &group, second_param, &currentGroup);
    	}

		else if((strcmp(comando, "cd")) == 0) {
			//utilizar pilha
			strcpy(diretorio,second_param);
			change_directory(second_param, &inode, &group, &currentGroup, &stack);
		}
		else if((strcmp(comando, "pwd")) == 0){
			pwd(&stack);
		}

		else if((strcmp(comando, "exit")) == 0){
			break;
		}
		else {
			printf("command not found.\n");
		}

	}



	close(fd);
	exit(0);
}
