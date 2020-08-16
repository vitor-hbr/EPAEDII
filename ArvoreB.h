#ifndef MY_HEADER_A
#define MY_HEADER_A

// - t correponde ao "grau minimo da arvore"
// - Todo no diferente da raiz deve ter pelo menos t-1 chaves.
//   Consequentemente, todo no interno tem pelo menos t filhos.
// - Se a arvore for nao vazia, a deve conter pelo menos 1 chave.
// - Todo no pode conter no maximo 2t - 1 chaves (no "completo").
//   Consequentemente, todo no interno pode ter no maximo 2t filhos.
#include "FileHandler.h"
#include "Index.h"

#define b_tree_t 10

typedef struct Chave {
   long long int cpf;
   int address;
} Chave;

typedef struct No {
   int nChaves;
   Chave chaves[2*b_tree_t -1];
   int filhos[2*b_tree_t];
   bool folha;
   int endereco;
} No;

typedef struct Arv_B {
   No* raiz;
} Arv_B;

typedef struct No_Indice {
   No* x;
   int i;
} No_Indice;


void setTreeRoot(Arv_B* T);
No_Indice b_tree_search(No* x, long long int k, No** ant, int* indexOfChild);
void b_tree_create(Arv_B* T);
void b_tree_insert(Arv_B* T,long long int k, int address);

void b_tree_insert_nonfull(No* x,long long int k, int address);
void b_tree_split_child(No* x, int i, No* y);
No* allocate_node(bool folha);

void b_tree_delete(No* x, int k, No* pai, int indexOfChild, Arv_B* root);
Chave findInOrderPre(No* no, int index);
Chave findInOrderPreForce(No* no, int index, No* pai, int indexOfChild, No* x, int k, Arv_B* root);
Chave findInOrderSucc(No* no, int index);
Chave removeFromNo(No* x, int k);
void fixNo(No* x, int k, No* pai, int indexOfChild, Arv_B* root);
void fixRoot(No* newRoot, Arv_B* root);
void fixNoPointers(No* no, int index);
void merge(No* leftNo, No* rightNo, Chave c);
Chave borrowFromLeft(No* no, int index, int* noFilho);
Chave borrowFromRight(No* no, int index, int* noFilho);

void b_tree_update(Arv_B* T, long long int cpf, int newAddress);

No_Indice _No_Indice(No* x, int i);

int calculateSizeofNO(bool folha);
No* readNoFromBuffer(Buffer* buffer);
void readChavesFromBuffer(Buffer* buffer, int nChaves, Chave* chaves);
void writeNo(No* no);


#endif