#include "utils.h"

typedef char Registro;

int POP(Pilha*);
int PUSH(const char*, Pilha*);
int TOP(Pilha*, char*);

int mostra(Pilha*, const char*);
Pilha* cria(size_t limite);
Pilha* destroi(Pilha* pilha);


// tira um cara da pilha, retorna o tamanho
int POP(Pilha* p)
{
    if (p == NULL) return -1;
    if (p->tam == 0) return -2;
    p->tam -= 1;
    return p->tam;
}

// poe um cara na pilha, retorna o tamanho
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

/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TAMANHO_ 1000
#define LIMITE_ 4000

typedef char Registro;

typedef struct
{
    int  tam; // quantos tem
    int  lim; // quantos cabem
    char dado[LIMITE_][TAMANHO_];

} Pilha;

int POP(Pilha*);
int PUSH(const char*, Pilha*);
int TOP(Pilha*, char*);

int mostra(Pilha*, const char*);
Pilha* cria(size_t limite);
Pilha* destroi(Pilha* pilha);

int main(void)
{
    Pilha orig = {.tam = 0, .lim = TAMANHO_};
    PUSH("(const char*)aa[0]", &orig);
    PUSH("livros", &orig);
    PUSH("clasicos", &orig);
    PUSH("(julio", &orig);
    POP(&orig);
    mostra(&orig, NULL);
    return 0;
}

*/