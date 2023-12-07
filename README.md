<h1> Implementação do Sistema de Arquivos EXT2 </h1>

Este programa implementa as estruturas de dados e operações para manipular a imagem (``.iso``) de um sistema de arquivos EXT2. As
operações são invocadas a partir de um prompt (shell). O shell executa as operações a partir da referência
do diretório corrente. O shell sempre inicia no raiz (``/``) da imagem informada por parâmetro.

As estruturas e a lógica de manipulação foram implementadas a partir da especificação do ext2:
https://www.nongnu.org/ext2-doc/ext2.html


<h3>Operações:</h3>

- `info`: exibe informações do disco e do sistema de arquivos.
- `cat <filename>`: exibe o conteúdo de um arquivo no formato texto.
- `attr <file | dir>`: exibe os atributos de um arquivo (*file*) ou diretório (*dir*).
- `cd <path>`: altera o diretório corrente para o definido como *path*.
- `ls`: listar os arquivos e diretórios do diretório corrente.
- `pwd`: exibe o diretório corrente (caminho absoluto).
- `cp '<source_path>' '<target_path>'`: copia um arquivo de origem (*source_path*) para destino (*target_path*). A origem e o destino devem ser uma partição do disco e o volume ext2 ou vice-versa. Note que os nomes de ambos arquivos precisam necessariamente estar entre aspas simples.
<br>


<h3>Para compilar:</h3>

- Descompacte a imagem ``myext2image.tar.gz`` e verifique sua integridade. Caso estiver corrompida, por favor realize novamente o download da imagem (isso pode ser feito [aqui](https://github.com/campiolo/ext2cat/tree/main)
) ou gere sua própria imagem ext2. E, por fim, a insira no diretório raiz deste projeto.

```bash
# utilizando o makefile
make super
```

```bash
# sem makefile
gcc ext2.c
```

<h3>Para executar:</h3>
<!-- ```console
# ./ext2cat <ext2-image-file>
``` -->

```bash
# utilizando o makefile
make debug
```

```bash
# sem makefile
./a.out
```

<br>


## Geração da Imagem de Volume Ext2

Gerando imagens ext2 (64MiB com blocos de 1K):
```console
dd if=/dev/zero of=./myext2image.img bs=1024 count=64K
mkfs.ext2 -b 1024 ./myext2image.img
```

Verificando a integridade de um sistema ext2:
```console
e2fsck myext2image.img
```

Montando a imagem do volume com ext2:
```console
sudo mount myext2image.img /mnt
```

Estrutura original de arquivos do volume (comando `tree` via bash):
```
/
├── [1.0K]  documentos
│   ├── [1.0K]  emptydir
│   ├── [9.2K]  alfabeto.txt
│   └── [   0]  vazio.txt
├── [1.0K]  imagens
│   ├── [8.1M]  one_piece.jpg
│   ├── [391K]  saber.jpg
│   └── [ 11M]  toscana_puzzle.jpg
├── [1.0K]  livros
│   ├── [1.0K]  classicos
│   │   ├── [506K]  A Journey to the Centre of the Earth - Jules Verne.txt
│   │   ├── [409K]  Dom Casmurro - Machado de Assis.txt
│   │   ├── [861K]  Dracula-Bram_Stoker.txt
│   │   ├── [455K]  Frankenstein-Mary_Shelley.txt
│   │   └── [232K]  The Worderful Wizard of Oz - L. Frank Baum.txt
│   └── [1.0K]  religiosos
│       └── [3.9M]  Biblia.txt
├── [ 12K]  lost+found
└── [  29]  hello.txt

```

Desmontando a imagem do volume com ext2:
```console
sudo umount /mnt
```

<br>

## Exemplos

Os exemplos são executados na imagem `myext2image.img` 

Informações do volume e do ext2 (comando `info`):
```console
[/]$> info
Volume name.....: SO-UTFPR-1k
Image size......: 67108864 bytes
Free space......: 32133 KiB
Free inodes.....: 16355
Free blocks.....: 35409
Block size......: 1024 bytes
Inode size......: 128 bytes
Groups count....: 8
Groups size.....: 8192 blocks
Groups inodes...: 2048 inodes
Inodetable size.: 256 blocks
```
<br>


Listagem do diretório raiz (comando `ls`):
```console
[/]$> ls
.
inode:      	2
Record lenght: 12
Name lenght: 1
File type: 2

..
inode:      	2
Record lenght: 12
Name lenght: 2
File type: 2

lost+found
inode:     	11
Record lenght: 20
Name lenght: 10
File type: 2

documentos
inode:  	12289
Record lenght: 20
Name lenght: 10
File type: 2

livros
inode:   	8193
Record lenght: 16
Name lenght: 6
File type: 2

imagens
inode:   	4097
Record lenght: 16
Name lenght: 7
File type: 2

hello.txt
inode:     	12
Record lenght: 928
Name lenght: 9
File type: 1

```

 

## Bibliotecas Utilizadas

- `time.h`: Oferece funções e constantes para manipulação do tempo, incluindo datas e horas.

- `sys/types.h`: Define símbolos e estruturas de typedef para operações diversas.

- `sys/stat.h`: Declara funções relacionadas a manipulação de arquivos no ambiente UNIX.

- `sys/wait.h`: Contém macros utilizadas para controle de processos.

- `sys/resource.h`: Oferece definições para operações de recursos conforme o padrão XSI.

- `fcntl.h`: Define requisições e argumentos para as funções `fcntl()` e `open()`, úteis para manipulação de arquivos.

- `unistd.h`: Fornece acesso à API do sistema operacional POSIX, facilitando operações de sistema.

- `errno.h`: Retorna valores para indicar falhas em funções.

- `./ext2_fs.h`: Contém estruturas de typedef relacionadas ao sistema de arquivos EXT2.




### :mortar_board: Autores

<table style="flex-wrap: wrap; display: flex; align-items: center;  flex-direction: column;" ><tr>


<td align="center"><a href="https://github.com/amandaferrari20">
 <img style="border-radius: 50%;" src="https://avatars.githubusercontent.com/u/86694266?v=4" width="100px;" alt=""/>
<br />
 <b>Amanda<br>Ferrari</b></a>
 <a href="https://github.com/amandaferrari20" title="Repositorio Amanda Ferrari"></a>
</td>


<td align="center"><a href="https://github.com/eniira">
 <img style="border-radius: 50%;" src="https://avatars.githubusercontent.com/u/102331777?v=4" width="100px;" alt=""/>
<br />
 <b>Catarine<br>Cruz
</b>
 </a> <a href="https://github.com/eniira" title="Repositorio Catarine Cruz"></a>
</td>


<td align="center"><a href="https://github.com/GustavoMartinx">
 <img style="border-radius: 50%;" src="https://avatars.githubusercontent.com/u/90780907?v=4" width="100px;" alt=""/>
<br />
 <b>Gustavo<br>Martins</b>
 </a> <a href="https://github.com/GustavoMartinx" title="Repositorio Gustavo Martins"></a>
</td>

</tr></table>