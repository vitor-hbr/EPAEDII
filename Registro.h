#ifndef MY_HEADER_REG
#define MY_HEADER_REG

#include <cstring>
#include <string>

#include "Header.h"
#include "Index.h"
#include "FileHandler.h"

using namespace std;

typedef struct Registry {
  int tamanho;
  long long int cpf;
  string nome;
  string sobrenome;
  string curso;
  char sexo;
  int data_nascimento;
  int address;
  bool vazio;
} Registry;

char* transformStringIntoChar(string str);
long long int readCpfFromCommand(string commandLine, int posChar);
Registry readRegistryFromCommand(string commandLine, int posChar);
int calculateByteSize(Registry reg);
bool getEmptySpace(int numberOfBytes, int* address);
void writeRegistry(Registry reg);
Registry readRegistry(long long int cpf);
void fixHeader(int numberOfBytes, Buffer buffer, Header* oldHeader);
void removeRegistry(long long int cpf);
void updateRegistry(Registry reg);
void writeReg(Registry reg);
#endif