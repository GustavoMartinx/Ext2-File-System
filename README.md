<h1> Implementação do Sistema de Arquivos EXT2 </h1>

-Primeiramente, verifique se a imagem myext2image.img está na pasta "implementação", caso não esteja, por favor realize o download da imagem e a insira dentro da pasta "implementação".

-Como Compilar:<br>
Utilizando o arquivo makefile:<br>
 Apenas digitar "make super"<br>
Sem o makefile:<br>
 gcc "nome_do_arquivo".c

-Como executar:<br>
Utilizando o arquivo makefile:<br>
 Apenas digitar "make debug" <br>
Sem o makefile:<br>
 ./a.out

-Bibliotecas usadas:<br>
   time.h, essa biblioteca contém diversas funções e constantes para a manipulação do tempo, como datas, horas e etc.<br>
   sys/types.h, define uma coleção de símbolos e estruturas de typedef.<br>
   sys/stat.h, declara funções relacionadas ao arquivo UNIX.<br>
   sys/wait.h, contém um conjunto de macros, usadas para manter processos.<br>
   sys/resource.h, contém definições para operações de recursos XSI.<br>
   fcntl.h, define requisições e argumentos para uso das funções fcntl() e open().<br>
   unistd.h, fornece acesso à API do sistema operacional POSIX.<br>
   errno.h, retorna um valor para indicar se uma função falhou.<br>
   ./ext2_fs.h, fornece estruturas de typedef relacionadas ao EXT2.<br>
   

-Exemplo de uso:<br>
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

-OBS.:<br>
Para utilizar o comando cp, basta digitar:<br>
cp 'arquivo_origem.txt' 'arquivo_destino.txt'<br>
Note que ambos arquivos precisam necessariamente estar entre aspas simples.
