
#include <stdlib.h>
#include <iostream>
#include <sys/stat.h>
#include "ArvoreB.h"


using namespace std;

void setTreeRoot(Arv_B* T) {
      struct stat sta;
      if(T->raiz == NULL){
            if(stat("Index.txt", &sta) == 0){
                  if(sta.st_size > 0){
                        Buffer* buffer = (Buffer*) malloc(sizeof(Buffer));
                        readBlock(buffer, sizeof(int), true);
                        int end = readIntFromBuffer(buffer);
                        buffer = readNoFromFile(end);
                        T->raiz = readNoFromBuffer(buffer);
                        T->raiz->endereco = end;
                        free(buffer);
                  }  
            }
      }
}

void b_tree_create(Arv_B* T) {
      pointToStartIndexFile();
      No* x = allocate_node(true);
      writeInt(x->endereco, sizeof(int), true);
      writeNo(x);
      T->raiz = x;
}

No* allocate_node(bool folha) {
      No* x = (No*) malloc (sizeof(No));
      x->folha = folha;
      x->nChaves = 0;
      x->endereco = getAddressEndOfIndexFile();
      return x;
}

No_Indice _No_Indice(No* x, int i) {
      No_Indice ni;
      ni.x = x;
      ni.i = i;
      return ni;
}

No_Indice b_tree_search(No* x, long long int k, No** ant, int* indexOfChild) {
      int i = 0;
      No* temp = NULL;
      while(i < x->nChaves && k > x->chaves[i].cpf) {          
            i++;
      }
      if(i < x->nChaves && k == x->chaves[i].cpf) {
            return _No_Indice(x, i);
      }
      if(x->folha) {
            return _No_Indice(temp, -1);
      } else {
            Buffer* buffer = readNoFromFile(x->filhos[i]);
            No* no = readNoFromBuffer(buffer);
            no->endereco = x->filhos[i];
            if(ant != NULL || indexOfChild != NULL){
                  *ant = x;
                  *indexOfChild = i;
            }
            return b_tree_search(no, k, ant, indexOfChild);
      }
}

// y eh o i-esimo filho de x e eh o no que esta sendo dividido
void b_tree_split_child(No* x, int i, No* y) {
      // --- cria o no z e copia a "metade direita" de y para z ---
      No* z = allocate_node(y->folha);
      z->nChaves = b_tree_t - 1;
      // copia as chaves da "metade direita" de y para z
      for(int j = 0; j < b_tree_t - 1; j++) {
            z->chaves[j] = y->chaves[j + b_tree_t];
      }

      // se tiver filhos, copia os filhos da "metade direita" de y para z
      if(!y->folha) {
            for(int j = 0; j < b_tree_t; j++) {
                  z->filhos[j] = y->filhos[j + b_tree_t];
            }
      }

      y->nChaves = b_tree_t - 1;
      // ----------------------------------------------------------

      // --- faz um "shift" de uma posição em c[i+1 .. n] para ---
      // --- caber a mediana do no y que esta sendo dividido   ---
      for(int j = x->nChaves; j > i; j--) {
            x->filhos[j + 1] = x->filhos[j];
      }

      x->filhos[i+1] = z->endereco;

      for(int j = x->nChaves - 1; j >= i; j--) {
            x->chaves[j + 1] = x->chaves[j];
      }

      x->chaves[i] = y->chaves[b_tree_t - 1];
      x->nChaves++;
      // -------------------------------------------------------------
      
      writeNo(y);
      writeNo(x);
      writeNo(z);
}

void b_tree_insert(Arv_B* T,long long int k, int address) {
      if(T->raiz == NULL){
            b_tree_create(T);
      }
      No* r = T->raiz;
      if(r->nChaves == (2 * b_tree_t - 1)) {
            No* s = allocate_node(false);
            T->raiz = s;
            s->filhos[0] = r->endereco;
            pointToStartIndexFile();
            writeBlock(&(s->endereco),sizeof(int), true);
            pointToIndexAddress(s->endereco);
            writeNo(s);
            b_tree_split_child(s, 0, r);
            b_tree_insert_nonfull(s, k, address);
      } else {
            b_tree_insert_nonfull(r, k, address);
      }
}

void b_tree_delete(No* x, int k, No* pai, int indexOfChild, Arv_B* root){
      if(x->folha){
            if(x->nChaves > b_tree_t-1 || x == root->raiz){
                  removeFromNo(x,k);
                  pointToIndexAddress(x->endereco);
                  writeNo(x); 
            } 
            else {
                  fixNo(x, k, pai, indexOfChild, root);
            }    
      } else {
            Chave ch;
            ch = findInOrderPre(x, k);
            if(ch.address == -1)
                  ch = findInOrderSucc(x,k);
            if(ch.address != -1){
                  x->chaves[k] = ch;
                  pointToIndexAddress(x->endereco);
                  writeNo(x); 
            } else {
                  No* pai = (No*) malloc(sizeof(No));
                  No* no = (No*) malloc(sizeof(No));
                  no = x;
                  ch = findInOrderPreForce(no, k, pai, indexOfChild, x, k, root);
            }     
      }
}

Chave findInOrderPre(No* no, int index){
      Buffer* buf;
      No* nextNo;
      Chave c;
      if(no->folha){
            if(no->nChaves <= b_tree_t-1){
                  c.address = -1;
            } else {  
                  c = removeFromNo(no, no->nChaves-1);
                  pointToIndexAddress(no->endereco);
                  writeNo(no);
            }
            return c;
      }
      if(index != -1){
            buf = readNoFromFile(no->filhos[index]);
            nextNo = readNoFromBuffer(buf);
            nextNo->endereco = no->filhos[index];
      } else{
            buf = readNoFromFile(no->filhos[no->nChaves]);
            nextNo = readNoFromBuffer(buf);
            nextNo->endereco = no->filhos[no->nChaves];
      }
      return findInOrderPre(nextNo, -1);
}

Chave findInOrderPreForce(No* no, int index, No* pai, int indexOfChild, No* x, int k, Arv_B* root){
      Buffer* buf;
      No* nextNo;
      Chave c;
      if(no->folha){
            long long int cpf = no->chaves[no->nChaves-1].cpf;
            c = no->chaves[no->nChaves-1];
            x->chaves[k] = c;
            pointToIndexAddress(x->endereco);
            writeNo(x);
            fixNo(no, no->nChaves-1, pai, indexOfChild, root);
            return c;
      }
      if(index != -1){
            buf = readNoFromFile(no->filhos[index]);
            nextNo = readNoFromBuffer(buf);
            nextNo->endereco = no->filhos[index];
            indexOfChild = no->nChaves;
      } else{
            buf = readNoFromFile(no->filhos[no->nChaves]);
            nextNo = readNoFromBuffer(buf);
            nextNo->endereco = no->filhos[no->nChaves];
            indexOfChild = no->nChaves;
      }
      pai = no;
      return findInOrderPreForce(nextNo, -1, pai , indexOfChild, x, k, root);
}

Chave findInOrderSucc(No* no, int index){
      Buffer* buf;
      No* nextNo;
      Chave c;
      if(no->folha){
            if(no->nChaves <= b_tree_t-1){
                  c.address = -1;
            } else {   
                  c = removeFromNo(no, 0);
                  pointToIndexAddress(no->endereco);
                  writeNo(no);
            }
            return c;
      }
      if(index != -1){
            buf = readNoFromFile(no->filhos[index+1]);
            nextNo = readNoFromBuffer(buf);
            nextNo->endereco = no->filhos[index+1];
      } else{
            buf = readNoFromFile(no->filhos[0]);
            nextNo = readNoFromBuffer(buf);
            nextNo->endereco = no->filhos[index+1];
      }
      return findInOrderSucc(nextNo, -1);
}

Chave removeFromNo(No* x, int k){
      Chave ch = x->chaves[k];
      x->nChaves--; 
      for(int i = k; i < x->nChaves; i++){
            x->chaves[i] = x->chaves[i+1];
      }
      return ch;
}

void fixNo(No* x, int k, No* pai, int indexOfChild, Arv_B* root){
      int extraNo = 1;
      if(pai != NULL)
       { 
            Chave c = borrowFromLeft(pai, indexOfChild-1, &extraNo);
            if(c.address != -1){
                  for(int i = k; i > 0; i--){
                        x->chaves[i] = x->chaves[i-1];
                  }
                  if(extraNo != -1){
                    for(int i = x->nChaves+1; i > 0; i--){
                        x->filhos[i] = x->filhos[i-1];
                    }  
                    x->filhos[0] = extraNo;  
                  }
                  x->chaves[0] =  pai->chaves[indexOfChild-1];
                  pai->chaves[indexOfChild-1] = c;
                  writeNo(pai);
                  writeNo(x);
            } else {
                  c = borrowFromRight(pai, indexOfChild+1, &extraNo);
                  if(c.address != -1) {
                  for(int i = k; i < x->nChaves-1; i++){
                        x->chaves[i] = x->chaves[i+1];
                  }
                  if(extraNo != -1 && !x->folha){ 
                    x->filhos[x->nChaves+1] = extraNo;  
                  }
                  x->chaves[x->nChaves-1] = pai->chaves[indexOfChild];
                  pai->chaves[indexOfChild] = c; 
                  writeNo(pai);
                  writeNo(x); 
            } else {
                  int posPai = indexOfChild;
                  No* irmao;
                  Buffer* buf;
                  if(indexOfChild != 0){
                        posPai--;
                        buf = readNoFromFile(pai->filhos[indexOfChild-1]);
                        irmao = readNoFromBuffer(buf); 
                        irmao->endereco = pai->filhos[indexOfChild-1];
                        c = pai->chaves[posPai];
                        removeFromNo(x, k);
                        fixNoPointers(pai, posPai);
                        merge(irmao, x, c);
                        pointToIndexAddress(irmao->endereco);
                        writeNo(irmao);
                        pointToIndexAddress(pai->endereco);
                        writeNo(pai);
                        if(pai->endereco == root->raiz->endereco && pai->nChaves == 0){
                              fixRoot(irmao, root);
                              pai = irmao;
                        }
                  } else {
                        buf = readNoFromFile(pai->filhos[indexOfChild+1]);
                        irmao = readNoFromBuffer(buf); 
                        irmao->endereco = pai->filhos[indexOfChild+1];
                        c = pai->chaves[posPai];
                        removeFromNo(x, k);
                        fixNoPointers(pai, posPai);
                        merge(x, irmao, c);
                        pointToIndexAddress(x->endereco);
                        writeNo(x);
                        pointToIndexAddress(pai->endereco);
                        writeNo(pai);
                        if(pai->endereco == root->raiz->endereco && pai->nChaves == 0){
                              fixRoot(x, root);
                              pai = x;
                        }
                  }
                  if(pai->nChaves < b_tree_t-1 && pai->endereco != root->raiz->endereco){
                        x = pai;
                        b_tree_search(root->raiz, x->chaves[0].cpf, &pai, &indexOfChild);     
                        fixNo(x, posPai, pai, indexOfChild, root);
                  }
            }
      }
      }   
}

void fixRoot(No* newRoot, Arv_B* root){
      pointToStartIndexFile();
      writeInt(newRoot->endereco, sizeof(int), true);
      root->raiz = newRoot;
}

void fixNoPointers(No* no, int index){
      for(int i = index+1; i < no->nChaves-1;i++){
            no->chaves[i] = no->chaves[i+1];
      }
}

void merge(No* leftNo, No* rightNo, Chave c){
      leftNo->chaves[leftNo->nChaves] = c;
      leftNo->nChaves++;
      for(int i = 0; i < rightNo->nChaves; i++){
           leftNo->chaves[leftNo->nChaves] = rightNo->chaves[i];
           leftNo->nChaves++;
     } 
}

Chave borrowFromLeft(No* no, int index, int* noFilho){
      Chave ch;
      ch.address = -1;
      if(index < 0 || index >= 2*b_tree_t || index > no->nChaves) return ch; 
      Buffer* buf = readNoFromFile(no->filhos[index]);
      No* irmao = readNoFromBuffer(buf);
      irmao->endereco = no->filhos[index];
      if(irmao->nChaves > b_tree_t-1){
            ch = irmao->chaves[irmao->nChaves-1];
            if(irmao->folha) {
                  *noFilho = -1;
            } else {
                  *noFilho = irmao->filhos[irmao->nChaves]; 
            }
            irmao->nChaves--;
            writeNo(irmao);
      }
      return ch;
}

Chave borrowFromRight(No* no, int index, int* noFilho){
      Chave ch;
      ch.address = -1;
      if(index < 0 || index >= 2*b_tree_t || index > no->nChaves) return ch; 
      Buffer* buf = readNoFromFile(no->filhos[index]);
      No* irmao = readNoFromBuffer(buf);
      irmao->endereco = no->filhos[index];
      if(irmao->nChaves > b_tree_t-1){
            if(!irmao->folha) {
                  *noFilho = -1;
            } else {
                  *noFilho = irmao->filhos[0]; 
            }
            ch = removeFromNo(irmao, 0);
            writeNo(irmao);
      }
      return ch;
}

void b_tree_update(Arv_B* T,long long int cpf, int newAddress){
      No_Indice no = b_tree_search(T->raiz, cpf, NULL, NULL);
      pointToIndexAddress(no.x->endereco + 1 + sizeof(int) + (sizeof(int) + sizeof(long long int)) * no.i + sizeof(long long int));
      writeInt(newAddress, sizeof(int), true);
}

void b_tree_insert_nonfull(No* x,long long int k, int address) {
      int i = x->nChaves-1;
      if(x->folha) {
            while(i >= 0 && k < x->chaves[i].cpf) {
                  x->chaves[i+1] = x->chaves[i];
                  i--;
            }
            x->chaves[i+1].cpf = k;
            x->chaves[i+1].address = address;
            x->nChaves++;
            writeNo(x);
      } else {
            while(i >= 0 && k < x->chaves[i].cpf) {
                  i--;
            }
            i++;
            Buffer* buffer = readNoFromFile(x->filhos[i]);
            No* no = readNoFromBuffer(buffer);
            no->endereco = x->filhos[i];
            if(no->nChaves == 2 * b_tree_t - 1) {
                  b_tree_split_child(x, i, no);
                  if(k > x->chaves[i].cpf) {
                        i++;
                  }
                  buffer = readNoFromFile(x->filhos[i]);
                  buffer->prox;
                  no = readNoFromBuffer(buffer);
                  no->endereco = x->filhos[i];
            }
            b_tree_insert_nonfull(no, k, address);
      }
}

int calculateSizeofNO(bool folha) {
      if(folha) return sizeof(bool) + (sizeof(int) + sizeof(long long int)) *(2 * b_tree_t - 1) +  sizeof(int);  
      return sizeof(bool) + sizeof(int) * (2 * b_tree_t + 1) + (sizeof(int) + sizeof(long long int)) *(2 * b_tree_t - 1); 
}

void readChavesFromBuffer(Buffer* buffer, int nChaves, Chave* chaves){
  for(int i = 0; i < nChaves; i++){
    chaves[i].cpf = readLongLongIntFromBuffer(buffer);
    chaves[i].address = readIntFromBuffer(buffer);
  }
  for(int i = 0; i < (2*b_tree_t - 1) - nChaves; i++){
    buffer->prox += sizeof(int) + sizeof(long long int);
  }
}

No* readNoFromBuffer(Buffer* buffer) {
      buffer->prox=1;
      No* no = (No*) malloc(sizeof(No));
      no->folha = buffer->folha;
      no->nChaves = readIntFromBuffer(buffer);
      
      readChavesFromBuffer(buffer, no->nChaves, no->chaves); 
      if(!no->folha){
            readIntArrayFromBuffer(buffer, no->filhos, no->nChaves); 
      }
      return no;
}

void writeNo(No* no){
      char c = PIPE;
      Buffer* buffer = (Buffer*) malloc(sizeof(Buffer));
      buffer->prox = 0;
      pointToIndexAddress(no->endereco);
      writeBoolToBuffer(buffer, no->folha);
      writeIntToBuffer(buffer, no->nChaves);
      for(int i = 0; i < no->nChaves; i++){
            writeLongLongIntToBuffer(buffer, no->chaves[i].cpf);
            writeIntToBuffer(buffer, no->chaves[i].address);
      }
      for(int i = 0; i < (2*b_tree_t - 1) - no->nChaves ; i++){
            for(int j = 0; j < sizeof(long long int) + sizeof(int); j++){
                  writeSeparatorToBuffer(buffer);
            }
      }
      if(!no->folha){
            for(int i = 0; i < no->nChaves+1; i++){
                  writeIntToBuffer(buffer, no->filhos[i]);
            }
            for(int i = 0; i < 2*b_tree_t-(no->nChaves+1);i++){
                  for(int j = 0; j < sizeof(int); j++)
                        writeSeparatorToBuffer(buffer);
            }
      }
      writeBlock(buffer->data, buffer->prox, true);   
} 