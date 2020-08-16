#ifndef MY_HEADER_INDEX
#define MY_HEADER_INDEX

#include <cstring>
#include <string>

#include "FileHandler.h"
#include "ArvoreB.h"

using namespace std;

typedef struct Index {
  long long int cpf;
  int address;
} Index;

Index searchAddress(long long int cpf);
void writeIndex(Index index);
Index removeIndex(long long int cpf);
Index buildIndex (long long cpf, int address);
void updateIndex (long long cpf, int address);
void getRootIndex();
void freeRoot();
#endif