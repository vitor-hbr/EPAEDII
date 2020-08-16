#include <cstring>
#include <iostream>
#include <stdio.h>
#include "Index.h"

using namespace std;

Arv_B* T;

Index searchAddress(long long int cpf) {
  setTreeRoot(T);
  if(T->raiz == NULL){
    return buildIndex(-1, -1);
  }
  No_Indice no = b_tree_search(T->raiz, cpf, NULL, NULL);
  if(no.x == NULL){
    return(buildIndex(-1,-1));
  }
  return(buildIndex(no.x->chaves[no.i].cpf, no.x->chaves[no.i].address));
}

void writeIndex(Index index) {
    b_tree_insert(T, index.cpf, index.address);
}

Index removeIndex(long long int cpf){ 
  No* pai = (No*) malloc(sizeof(No));
  int indexOfChild = 0;
  No_Indice no = b_tree_search(T->raiz, cpf, &pai, &indexOfChild);
  Index index = buildIndex(no.x->chaves[no.i].cpf, no.x->chaves[no.i].address);
  b_tree_delete(no.x, no.i, pai, indexOfChild, T);
  return(index);
}

Index buildIndex(long long cpf, int address) {
  Index index;
  index.cpf = cpf;
  index.address = address;
  return index;
}

void updateIndex(long long cpf, int address){
  b_tree_update(T, cpf, address);
}

void getRootIndex() {
  openFiles();
  T = (Arv_B*) malloc(sizeof(Arv_B));
  T->raiz = NULL;
  setTreeRoot(T);
  closeFiles();
}

void freeRoot(){
  free(T);
}