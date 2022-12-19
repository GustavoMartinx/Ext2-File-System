Implementação de Sistema de Arquivos EXT2
Alunos: Amanda Ferrari, Catarine Cruz e Gustavo Zanzin. 

-Como Compilar:
Utilizando o arquivo makefile:
 Apenas digitar "make super"
Sem o makefile:
 gcc "nome_do_arquivo".c

-Como executar: 
Utilizando o arquivo makefile:
 Apenas digitar "make debug" 
Sem o makefile:
 ./a.out

-Bibliotecas usadas:
   time.h, essa biblioteca contém diversas funções e constantes para a manipulação do tempo, como datas, horas e etc.
   sys/types.h, define uma coleção de símbolos e estruturas de typedef.
   sys/stat.h, declara funções relacionadas ao arquivo UNIX.
   sys/wait.h, contém um conjunto de macros, usadas para manter processos.
   sys/resource.h, contém definições para operações de recursos XSI.
   fcntl.h, define requisições e argumentos para uso das funções fcntl() e open().
   unistd.h, fornece acesso à API do sistema operacional POSIX.
   errno.h, retorna um valor para indicar se uma função falhou.
   ./ext2_fs.h, fornece estruturas de typedef relacionadas ao EXT2.
   

-Exemplo de uso:
Utilizando o comando ls, por exemplo:
[/]$> ls

Obtemos o seguinte resultado:

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

-Exemplo de uso
Para utilizar o comando cp, basta digitar:
cp 'arquivo_origem.txt' 'arquivo_destino.txt'
Note que ambos arquivos precisam necessariamente estar entre aspas simples.
