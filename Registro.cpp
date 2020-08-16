#include <cstring>
#include <string>
#include <iostream>
#include <stdio.h>

#include "Registro.h"

using namespace std;

char* transformStringIntoChar(string str) {
  char* res = (char*) "";
  for(int i = 0; i < str.length(); i++){
    res[i] = str[i]; 
  }
  return res;
}

long long int readCpfFromCommand(string commandLine, int posChar) {
  string temporary("");
  for(int i = posChar + 1; i < commandLine.length() - 1; i++){
    temporary += commandLine[i];
  }
  return stoll(temporary, nullptr, 10);
}
 
Registry readRegistryFromCommand(string commandLine, int posChar) {
  Registry registry;
  int field = 0;
  registry.nome = "";
  registry.sobrenome = "";
  registry.curso = "";
  string temporary("");
  for(int i = posChar + 1; i < commandLine.length() - 1; i++){
    if(commandLine[i] == ','){
      if(field == 0) registry.cpf = stoll(temporary, nullptr, 10);
      temporary = "";
      field++;
      continue;
    }
    if(field == 0){
      temporary += commandLine[i];
    } else if(field == 1) {
      registry.nome += commandLine[i];
    } else if(field == 2){
      registry.sobrenome += commandLine[i];
    } else if(field == 3){
      registry.curso += commandLine[i];
    } else if(field == 4){
      registry.sexo = commandLine[i];
    } else {
      temporary += commandLine[i];
    }
  }
  registry.data_nascimento = stoi(temporary, nullptr, 10);
  registry.tamanho = calculateByteSize(registry);
  return registry;
}

int calculateByteSize(Registry reg) {
  int size = sizeof(reg.cpf) + reg.curso.length() + 3 +
  sizeof(reg.data_nascimento) + reg.nome.length() + 
  reg.sobrenome.length() + sizeof(reg.tamanho) + sizeof(reg.sexo);
  return size;
}

bool getEmptySpace(int numberOfBytes, int* address) {
  Header* header = readHeader();
  int pointer = header->empty_list_address;
  while(pointer != 0) {
    Buffer* buffer = (Buffer*) malloc(sizeof(Buffer));
    readBlock(buffer, sizeof(int) * 2 + 1, false);
    int size = readIntFromBuffer(buffer);
    if(size >= numberOfBytes + 9){
      *address = pointer;
      fixHeader(numberOfBytes, *buffer, header);
      return true;
    } else {
      return false;
    }
    free(buffer);
  }
  return false;
}

void fixHeader(int numberOfBytes, Buffer buffer, Header* oldHeader) {
  int nextEmptyAddress = readIntFromBuffer(&buffer);
  Header* newHeader = (Header*) malloc(sizeof(Header));
  newHeader->occupied_bytes = oldHeader->occupied_bytes + numberOfBytes;
  newHeader->empty_bytes = oldHeader->empty_bytes - numberOfBytes;
  newHeader->empty_list_address = nextEmptyAddress;
  pointToStartRegistryFile();
  writeHeader(newHeader);
  free(newHeader);
}

void writeRegistry(Registry reg) {
  if(getEmptySpace(reg.tamanho, &reg.address)) {
    writeIndex(buildIndex(reg.cpf, reg.address));
    pointToRegistryAddress(reg.address);
  } else {
    int address = getAddressEndOfRegistryFile();
    writeIndex(buildIndex(reg.cpf, address));
    Header* header = readHeader();
    header->occupied_bytes += reg.tamanho;
    writeHeader(header);
    free(header);
    pointToRegistryAddress(address);
  }
  writeReg(reg);
}

Registry readRegistry(long long int cpf) {
  Index index = searchAddress(cpf);
  Registry registry;
  if(index.address == -1){
    registry.vazio = true;
    return registry;
  }
  string stringTemp = ("");
  pointToRegistryAddress(index.address);
  Buffer* buffer = (Buffer*) malloc(sizeof(Buffer));
  readBlock(buffer, BUFFER_SIZE, false);
  buffer->prox = 0;
  registry.tamanho = readIntFromBuffer(buffer);
  registry.cpf = readLongLongIntFromBuffer(buffer);
  readStringFromBuffer(buffer, &stringTemp);
  registry.nome = stringTemp;
  stringTemp = ("");
  readStringFromBuffer(buffer, &stringTemp);
  registry.sobrenome = stringTemp;
  stringTemp = ("");
  readStringFromBuffer(buffer, &stringTemp);
  registry.curso = stringTemp;
  registry.sexo = readCharFromBuffer(buffer);
  registry.data_nascimento = readIntFromBuffer(buffer);
  free(buffer);
  registry.vazio = false;
  return registry;
}

void removeRegistry(long long int cpf) {
  Index index = removeIndex(cpf);
  Header* header = readHeader();
  pointToRegistryAddress(index.address);
  Buffer* buffer = (Buffer*) malloc(sizeof(Buffer));
  readBlock(buffer, sizeof(int), false);
  int size =  readIntFromBuffer(buffer);
  pointToRegistryAddress(index.address);
  buffer->prox = 0;
  writeCharToBuffer(buffer, '*');
  writeIntToBuffer(buffer, size);
  writeIntToBuffer(buffer, header->empty_list_address);
  writeBlock(buffer->data, buffer->prox, false);
  header->empty_bytes += size;
  header->empty_list_address = index.address;
  pointToStartRegistryFile();
  writeHeader(header);
  free(buffer);
}

void updateRegistry(Registry reg) {
  Index index = searchAddress(reg.cpf);
  Buffer* buffer = (Buffer*) malloc (sizeof(Buffer));
  buffer->prox = 0;
  if(index.address == -1){
    cout << "ERRO NO NAO ENCONTRADO";
    exit(0);
  }
  int newSize = calculateByteSize(reg);
  Header* header = readHeader();
  pointToRegistryAddress(index.address);
  int size;
  readBlock(buffer, sizeof(int), false);
  size = readIntFromBuffer(buffer);
  buffer->prox = 0;
  if(newSize == size){
    pointToRegistryAddress(index.address);
  } else if(newSize + 1 + sizeof(int) + sizeof(int) < size) {
    pointToRegistryAddress(index.address + newSize);
    writeCharToBuffer(buffer, '*');
    writeIntToBuffer(buffer, size-newSize);
    writeIntToBuffer(buffer, header->empty_list_address);
    writeBlock(buffer->data, buffer->prox, false);  
    header->empty_bytes += size - newSize;
    header->occupied_bytes += newSize - size;
    header->empty_list_address = index.address + newSize;
    pointToStartRegistryFile();
    writeHeader(header);
    pointToRegistryAddress(index.address);
  } else {
    pointToRegistryAddress(index.address);
    writeCharToBuffer(buffer, '*');
    writeIntToBuffer(buffer, size);
    writeIntToBuffer(buffer, header->empty_list_address);
    writeBlock(buffer->data, buffer->prox, false);  
    header->empty_bytes += size;
    header->occupied_bytes += newSize;
    header->empty_list_address = index.address;
    pointToStartRegistryFile();
    writeHeader(header);
    int address = getAddressEndOfRegistryFile();
    pointToRegistryAddress(address);
    updateIndex(reg.cpf, address);
  }
  writeReg(reg);
  free(buffer);
}

void writeReg(Registry reg){
  Buffer* buffer = (Buffer*) malloc (sizeof(Buffer));
  buffer->prox = 0;
  writeIntToBuffer(buffer, reg.tamanho);
  writeLongLongIntToBuffer(buffer, reg.cpf);
  writeStringToBuffer(buffer, reg.nome);
  writeSeparatorToBuffer(buffer);
  writeStringToBuffer(buffer, reg.sobrenome);
  writeSeparatorToBuffer(buffer);
  writeStringToBuffer(buffer, reg.curso);
  writeSeparatorToBuffer(buffer);
  writeCharToBuffer(buffer, reg.sexo);
  writeIntToBuffer(buffer, reg.data_nascimento);
  writeBlock(buffer->data, buffer->prox, false);
  free(buffer);
}