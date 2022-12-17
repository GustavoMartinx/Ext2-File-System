all:	
	clear
	gcc utils.h ext2.c
	./a.out

minishell:
	minishell.c -o mini
	./mini