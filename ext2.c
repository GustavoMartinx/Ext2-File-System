/*
Descrição: O código a seguir, tem o objetivo de simular o sistema de arquivos Ext2.
Autores: Amanda Ferrari, Catarine Cruz e Gustavo Zanzin.
Data de criação: 
Atualizado em: 09/12/2022, 10/12/2022, 16/12/2022, 17/12/2022.
*/

#include "./utils.h"

static unsigned int block_size = 0;        /* block size (to be calculated) */
struct ext2_super_block super;
int podeExecutar = 0;
int fd;

/******* Para controle **********/
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

int mostra(Pilha* p, const char* tit)
{
    if (p == NULL)
    {
        return -1;
    }
    if (tit != NULL) printf("%s\n", tit);
    if (p->tam == 0)
    {
		printf("/");
        return 0;
    }
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

		if((strcmp(nomeArquivo, entry->name)) != 0) {
			podeExecutar = 1;
		}

		free(block);
	}

	printf("\n\n");
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
			
			// Insere o caracter nulo para indicar o fim do nome do arquivo
			file_name[entry->name_len] = 0;
 
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
			entry = (void*) entry + entry->rec_len;
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
			"%d  ",
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
			);
		/*Colocar a unidade correta*/
	if (entry->i_size > 1024){
		printf("   %.1f KiB\t\t", (((float)entry->i_size) / 1024));
	}else{
		printf("    %d B\t\t", (entry->i_size));
	}
		/*Transformando os segundos em data e hora, pela biblioteca time.h*/
		struct tm *mtime;
			time_t segundos = entry->i_mtime;
			mtime = localtime(&segundos); 
			printf(
					"%d/"
					"%d/"
					"%d"
					" %d:" 
					"%d\n",
		   			mtime->tm_mday, 	
					mtime->tm_mon + 1, 
					(mtime->tm_year + 1900),
		   			mtime->tm_hour, 
					mtime->tm_min
				);

	
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

		//se não encontrar o diretorio:
		if((strcmp(dirName, entry->name)) != 0){
			printf("\ndirectory not found.");
		}
		
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


// Separa primeiro/principal comando do input completo.
char* catch_principal_param(char* comando) {
	
	char* buff = calloc(100, sizeof(char));

	for(int i = 0; comando[i] != ' '; i++) {
			
		buff[i] = comando[i];
	}

	return buff;
}

// Separa segundo comando do input completo
char* catch_second_param(char* comando) {

	if(strstr(comando, " ") != NULL) {

		char* forward_space_position = calloc(100, sizeof(char));
	    // Retorna o restante da string a partir de onde estiver o char " " (espaço)
        forward_space_position = strchr(comando, ' ');

		// Para remover o espaço que fica na primeira posição:
		// Copia cada elemento uma posição para trás 
		for(int j = 0; forward_space_position[j+1] != '\0'; j++) {
			forward_space_position[j] = forward_space_position[j+1];
		}
        
		// Com isso, o último char fica duplicado. Logo, insere-se
		// '\0' na última posição de forward_space_position
		int second_param_len = strlen(forward_space_position);
		forward_space_position[second_param_len - 1] = '\0';

		return forward_space_position;
	}
}

// Separa segundo comando/primeiro parâmetro (arquivo origem) do input completo de um comando cp
char* catch_second_param_cp(char* comando) {

	// Alocação de memória
    char* command_copy = calloc(100, sizeof(char));
    char* primeiro_entre_aspas = calloc(100, sizeof(char));
	strcpy(command_copy, comando);
    
    // Se existir aspas dupla na string (ou seja, dois arquivos/parâmetros para o cp), 
    if(strstr(command_copy, "'") != NULL) {

		// descarta a string até a ocorrência da primeira aspas
        strtok(command_copy, "'");
        // salva a string até a próxima (segunda) aspas => primeiro arquivo (origem)
        primeiro_entre_aspas = strtok(NULL, "'");
        // printf("primeiro_entre_aspas: %s\n", primeiro_entre_aspas);
       
        // Desalocação de memória na condição verdadeira
        //free(command_copy);

        return primeiro_entre_aspas;
	
    } else {
        
        // Desalocação de memória na condição falsa
        free(primeiro_entre_aspas);
        free(command_copy);

        // Mensagem para tratamento de erro
        printf("ERRO. Sintaxe inválida: insira o nome dos arquivos entre aspas simples.");
    }
}


// Separa segundo comando/terceiro parâmetro (arquivo de destino) do input completo de um comando cp
char* catch_third_param_cp(char* comando) {

    // Alocação de memória
    char* command_copy = calloc(100, sizeof(char));
    char* segundo_entre_aspas = calloc(100, sizeof(char));
	strcpy(command_copy, comando);

	if(strstr(command_copy, "'") != NULL) {

		// Descarta a string até a ocorrência da primeira aspas
        strtok(command_copy, "'");
        // Descarta a string até a próxima (segunda) aspas => primeiro arquivo (origem)
        strtok(NULL, "'");
        // Descarta a string até a ocorrência do terceiro parâmetro (terceira aspas)
        strtok(NULL, "'");
        // Salva a string até a próxima (quarta) aspas => segundo arquivo (destino)
        segundo_entre_aspas = strtok(NULL, "'");
        // printf("segundo_entre_aspas: %s\n", segundo_entre_aspas);

        // Desalocação de memória na condição verdadeira
        //free(command_copy);
	
        return segundo_entre_aspas;

    } else {
        
        // Desalocação de memória na condição falsa
        free(segundo_entre_aspas);
        free(command_copy);

        // Mensagem para tratamento de erro
        printf("ERRO. Sintaxe inválida: insira o nome dos arquivos entre aspas simples.");
		
    }
}


// Copia conteúdo dos blocos de dados nos inodes para o arquivo de destino.
void copia_arquivo(struct ext2_inode* inode, char* originFile, char* destinyFile, struct ext2_group_desc *group, int *currentGroup) {
	
	struct ext2_group_desc* grupoTemp = (struct ext2_group_desc *)malloc(sizeof(struct ext2_group_desc));
	struct ext2_inode* inodeEntryTemp = (struct ext2_inode*)malloc(sizeof(struct ext2_inode));
	
	memcpy(grupoTemp, group, sizeof(struct ext2_group_desc));
	memcpy(inodeEntryTemp, inode, sizeof(struct ext2_inode));
	unsigned int inodeRetorno = read_dir(fd, inodeEntryTemp, grupoTemp, originFile);

	change_group(&inodeRetorno, grupoTemp, currentGroup);
	
	int index = inodeRetorno % super.s_inodes_per_group;

	read_inode(fd, index, group, inodeEntryTemp);
	
	char* destinyFileName = malloc(strlen(destinyFile) + 2);
	strcat(destinyFileName, "./");
	strcat(destinyFileName, destinyFile);

	
	FILE *destinyFileFile;
	destinyFileFile = fopen(destinyFileName, "w");
  	
	if (destinyFileFile == NULL) {
  	  printf("Erro ao tentar abrir o arquivo!");
  	  //exit(1);
  	}

	// Aloca um bloco
	char *block = (char*)malloc(sizeof(char) * block_size);

	// Posiciona o leitor no primeiro bloco de dados diretos e o lê em block
	lseek(fd, BLOCK_OFFSET(inodeEntryTemp->i_block[0]), SEEK_SET);
	read(fd, block, block_size);

	int arqSize = inodeEntryTemp->i_size;
	int singleIndirection[256];
	int doubleIndirection[256];
	char charLido = '0';

	// Percorrendo pelos blocos de dados sem indireção
	for(int i = 0; i < 12; i++) {

		lseek(fd, BLOCK_OFFSET(inodeEntryTemp->i_block[i]), SEEK_SET);
		read(fd, block, block_size); // Lê bloco i em block
		
		// Exibindo conteúdo do primeiro bloco
		for(int i = 0; i < 1024; i++) {
			
			// Copia caracter da posição i de bloco para charLido
			charLido = block[i];

			// Escreve o caracter lido no arquivo de destino
			//printf("%c",block[i]);
			fputc(charLido, destinyFileFile);
			
			// Quantidade de dados restantes a serem lidos
			arqSize = arqSize - sizeof(char);

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
				
				// Copia caracter da posição i de bloco para charLido
				charLido = block[j];

				// Escreve o caracter lido no arquivo de destino
				//printf("%c",block[i]);
				fputc(charLido, destinyFileFile);
				
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
					
					// Copia caracter da posição i de bloco para charLido
					charLido = block[k];

					// Escreve o caracter lido no arquivo de destino
					//printf("%c",block[i]);
					fputc(charLido, destinyFileFile);
					
					arqSize = arqSize - 1;
					
					if (arqSize <= 0){
						break;
					}
				}
			}
		}
	}

	// Fazendo o inode voltar a ser o inode do diretório em que estava no momento em que
	// o cp de um arquivo foi chamado.
	unsigned int current_dir_entry_inode = read_dir(fd, inodeEntryTemp, grupoTemp, ".");

	read_inode(fd, current_dir_entry_inode, grupoTemp, inodeEntryTemp);

	free(block);
	free(grupoTemp);
	free(inodeEntryTemp);

	fclose(destinyFileFile);
}

//verifica se por exemplo foi digitado apenas o comando ou o comando e espaço para avisar erro de sintaxe
int verifica_sintaxe(char* segundoPar) {

	if(strcmp(segundoPar, "NULL") == 0 || strcmp(segundoPar, "") == 0 ) {
		printf("invalid sintax.\n");
		return 1;
	}
	return 0;
}


int main() {

	struct ext2_inode inode;
	struct ext2_group_desc group;
	int currentGroup = 0;
	
	// Criando a pilha 
	Pilha stack = {.tam = 0, .lim = TAMANHO_};

 	// Abrindo a imagem (floppy disk image)
 	if ((fd = open(FD_DEVICE, O_RDONLY)) < 0) {
 		perror(FD_DEVICE);
 		exit(1);  // error while opening the floppy device
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
		printf("[");
		mostra(&stack, NULL);
		printf("]$> ");

		// Captura comando completo pelo shell. Ex.: cat fileName
		fgets(fullCommand, 100, stdin);
    	fullCommand[strcspn(fullCommand, "\n")] = 0;

		// Alocações para comando principal e seu parâmetro (se houver)
		char* comando = calloc(100, sizeof(char));
		char* second_param = calloc(100, sizeof(char));
		char* third_param = calloc(100, sizeof(char));
		second_param = "NULL";
		third_param = "NULL";

		
		// Verificação para identificar se o comando digitado pelo usuário possui argumentos ou não:
        // Se existir o caractere " ' " (aspas simples) no comando principal - ou seja, o comando é o cp -,
        // então chama as funções para capturar o comando principal, o arquivo de origem e o arquivo de
        // destino, respectivamente
		if(strstr(fullCommand, "'") != NULL) {
			
			// Captura primeiro parâmetro (comando principal)  Ex.: cp
			comando = catch_principal_param(fullCommand);
		
			// Captura terceiro segundo (second_param)         Ex.: <fileOrigin>
			second_param = catch_second_param_cp(fullCommand);
			
            // Captura terceiro parâmetro (third_param)        Ex.: <fileDestiny>
            third_param = catch_third_param_cp(fullCommand);
			


		// Se existir um espaço no comando e não existir aspas (que nesse caso é a forma correta de usar o
        // comando cp, ou seja, o comando é diferente de cp), então chama as funções para capturar o
        // comando principal (primeiro - ex.: cat) e o segundo argumento (ex.: <fileName>), respectivamente.
		} else if(strstr(fullCommand, " ") != NULL) {
            
            // Captura comando principal (comando) 		  Ex.: cat
            comando = catch_principal_param(fullCommand);
            //printf("\ncomando principal la fora: %s\n", comando);

            // Captura segundo comando (second_param)     Ex.: <fileName>
            second_param = catch_second_param(fullCommand);

        
        // Caso contrário (ou seja, se não existir o char "espaço" ou " ' " (aspas simples) no comando
        // principal), apenas copia o conteúdo do comando principal (fullCommand) para a variável comando.
		} else if(strchr(fullCommand, ' ') == NULL) {
			
			// Captura comandos do tipo que são compostos por apenas uma palavra. Ex.: info
			strcpy(comando, fullCommand);
        }
		

		/****** Comparações: entrada == comando esperado  ******/
		if((strcmp(comando, "cd")) == 0) {
			podeExecutar = 0;
		
			if(strcmp(fullCommand,"cd")==0){
				printf("\ninvalid sintax.\n");
				podeExecutar = 1;
			}
			
			strcpy(diretorio,second_param);
			
			if(podeExecutar == 0) {
				change_directory(second_param, &inode, &group, &currentGroup, &stack);
			}
			podeExecutar = 0;
		}

    	else if((strcmp(comando, "info")) == 0) {
            info();
        }

        else if((strcmp(comando, "cat")) == 0) {

			read_dir(fd, &inode, &group, second_param);

			if(podeExecutar != 1){
				cat(fd, &inode, &group, second_param, &currentGroup);
			}

			if(strcmp(fullCommand,"cat") == 0) {
				printf("\ninvalid sintax.\n");
				podeExecutar = 0;
			}
			
			if(podeExecutar == 1){
				printf("\nfile not found.\n");
			}
			podeExecutar = 0;
        }

        else if((strcmp(comando, "attr")) == 0) {
            attr(&inode, &group, second_param, &currentGroup);
    	}

		else if((strcmp(comando, "pwd")) == 0) {
			pwd(&stack);
		}

		else if((strcmp(comando, "cp")) == 0) {
			copia_arquivo(&inode, second_param, third_param, &group, &currentGroup); // destinyFile => third_param | originFile => novo second_param ;
		}

		else if((strcmp(comando, "exit")) == 0){
			break;
		}

		else if((strcmp(comando, "ls")) == 0) {
	       ls(&inode, &group);
        }

		else {
			printf("command not found.\n");
		}

		// free(comando);
        // free(second_param);
        // free(third_param);
	}



	free(fullCommand);
	close(fd);
	exit(0);
}
