
#include <cstring>
#include <iostream>
#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <stdio.h>

#include "Header.h"
#include "ArvoreB.h"

using namespace std;

unsigned char PIPE_CHAR[2] = {PIPE, '\0'};
FILE *registryFile = NULL;
FILE *indexFile = NULL;

int getAddressEndOfRegistryFile() {
  pointToEndRegistryFile();
  long long int end = ftell(registryFile);
  if(end == 0){
    return 12;
  } else {
    return end;
  }
}

int getAddressEndOfIndexFile() {
  pointToEndIndexFile();
  long long int end = ftell(indexFile);
  if(end == 0){
    return 4;
  } else {
    return end;
  }
  
}

void pointToStartRegistryFile() {
  fseek(registryFile, 0, SEEK_SET);
}

void pointToStartIndexFile() {
  fseek(indexFile, 0, SEEK_SET);
}

void pointToRegistryAddress(int address) {
  fseek(registryFile, address, SEEK_SET);
}

void pointToIndexAddress(int address) {
  fseek(indexFile, address, SEEK_SET);
}

void pointToEndIndexFile() {
  fseek(indexFile, 0, SEEK_END);
}

void pointToEndRegistryFile() {
  fseek(registryFile, 0, SEEK_END);
}

void writeBlock(void * data, int numOfBytes, bool isIndexFile) {
  if(isIndexFile) fwrite(data, numOfBytes, 1, indexFile);
  else fwrite(data, numOfBytes, 1, registryFile);
}

void writeSeparatorToBuffer(Buffer* buffer) {
  buffer->data[buffer->prox] = PIPE;
  buffer->prox++;
}
        
void writeStringToBuffer(Buffer* buffer, string string) {
  for(int i = 0; i < string.length(); i++){
    buffer->data[buffer->prox++] = string.at(i);
  }
}

void writeInt(int n, int intSize, bool isIndexFile) { 
  writeBlock((unsigned char*)&(n), intSize, isIndexFile);
}

void writeIntToBuffer(Buffer* buffer, long long int number){ 
   for(int i = 0; i < sizeof(int); i++) {
      buffer->data[buffer->prox++] = number >> (8 * i);
   }
}

void writeLongLongIntToBuffer(Buffer* buffer,  long long int number){ 
   for(int i = 0; i < sizeof(long long int); i++) {
      buffer->data[buffer->prox++] = number >> (8 * i);
   }
}

void writeBoolToBuffer(Buffer* buffer,  bool b){ 
  buffer->data[buffer->prox++] = b;
}

void writeCharToBuffer(Buffer* buffer, char c) {
  buffer->data[buffer->prox++] = c;
}

void readBlock(Buffer* buffer, int numOfBytes, bool isIndexFile) {
  if(isIndexFile) fread(buffer->data, numOfBytes, 1, indexFile);
  else fread(buffer->data, numOfBytes, 1, registryFile);
  buffer->prox = 0;
}

Buffer* readRegistryFromFile(int numOfBytes, int address) {
  Buffer* buffer = (Buffer*) malloc(sizeof(Buffer));
  fseek(registryFile, address, SEEK_SET);
  fread(buffer->data, numOfBytes, 1, registryFile);
  buffer->prox = 0;
  return buffer;
}

Buffer* readNoFromFile(int address){
  int size = 0;
  Buffer* buffer = (Buffer*) malloc(sizeof(Buffer));
  buffer->prox = 0;
  pointToIndexAddress(address);
  fread(&buffer->folha, 1, 1, indexFile);
  pointToIndexAddress(address);
  size = calculateSizeofNO(buffer->folha);
  fread(buffer->data, size, 1, indexFile);
  buffer->prox = 1;
  return buffer;
}

long long int readLongLongIntFromBuffer(Buffer* buffer){ 
   long long int n = 0;
   for(int i = 0; i < sizeof(long long int); i++) {
      n += (long long int) (buffer->data[buffer->prox++]) << (8 * i);
   }
   return n;
}

int readIntFromBuffer(Buffer* buffer){ 
  int n = 0;
   for(int i = 0; i < sizeof(int); i++) {
      n += (int) (buffer->data[buffer->prox++]) << (8 * i);
   }
   return n;
}

void readStringFromBuffer(Buffer* buffer, string* s) {
    int i = 0;
    char str[30];
    while (buffer->data[buffer->prox] != PIPE) {
      str[i++] = buffer->data[buffer->prox++];
    }
    str[i] = '\0';
    *s = str;
    buffer->prox++;
}

bool readBoolFromBuffer(Buffer* buffer){
  return buffer->data[buffer->prox++];
}

char readCharFromBuffer(Buffer* buffer) {
  char c = buffer->data[buffer->prox];
  buffer->prox++;
  return c;
}

void readIntArrayFromBuffer(Buffer* buffer, int* filhos, int nChaves){
  for(int i = 0; i <= nChaves; i++){
    filhos[i] = (int) readIntFromBuffer(buffer);
  }
}

void openFiles() {
  if(registryFile != NULL)
  fclose(registryFile);
  registryFile = fopen("Registry.txt", "r+b");
  if(registryFile == NULL){
    fclose(registryFile);
    registryFile = fopen("Registry.txt", "w+b");
    Header* h = (Header*) malloc(sizeof(Header));
    h->occupied_bytes = 0;
    h->empty_bytes = 0;
    h->empty_list_address = 0;
    writeHeader(h);
    free(h);
  }
  indexFile = fopen("Index.txt", "r+b");
  if(indexFile == NULL){
    indexFile = fopen("Index.txt", "w+b");
  }
}

void closeFiles() {
  fclose(registryFile);
  fclose(indexFile);
}

void defragment() {
  int lastOkPoint = sizeof(int) * 2 + sizeof(int);
  int lastNonOkPoint = lastOkPoint;
  int size = 0;
  Header* header = readHeader();
  int end = getAddressEndOfRegistryFile();
  Buffer* buffer = (Buffer*) malloc(sizeof(Buffer));
  while(lastNonOkPoint < end) {
    pointToRegistryAddress(lastNonOkPoint);
    readBlock(buffer, 20, false);
    buffer->prox = 0;
    if(buffer->data[0] == '*'){
      buffer->prox++;
      size = readIntFromBuffer(buffer);
      lastNonOkPoint += size;
    } else {
      size = readIntFromBuffer(buffer);
      long long int cpf = readLongLongIntFromBuffer(buffer); 
      if(lastNonOkPoint != lastOkPoint){
        pointToRegistryAddress(lastNonOkPoint);
        readBlock(buffer, size, false);
        updateIndex(cpf, lastOkPoint);
        pointToRegistryAddress(lastOkPoint);
        writeBlock(buffer->data, size, false);
      } 
      lastNonOkPoint += size;
      lastOkPoint += size;
    }
  }
    free(buffer);
    header->occupied_bytes = lastOkPoint - 12;
    header->empty_bytes = 0;
    header->empty_list_address = 0;
    writeHeader(header);
    reWriteRegistryFile();
}

bool registryFileExists() {
  if (FILE *file = fopen("Registry.txt", "r")) {
    fclose(file);
    return true;
  } else {
    return false;
  }   
}

void reWriteRegistryFile(){
  FILE* newRegistryFile = fopen("Registry_Temp.txt", "w+b");
  Header* header = readHeader();
  Buffer* buffer = (Buffer*) malloc(sizeof(Buffer));
  buffer->prox = 0;
  writeIntToBuffer(buffer, header->occupied_bytes);
  writeIntToBuffer(buffer, header->empty_bytes);
  writeIntToBuffer(buffer, header->empty_list_address);
  fwrite(buffer->data,headerSize(), 1, newRegistryFile);
  int end = header->occupied_bytes;
  int pointer = 12;
  while(pointer < end){
    pointToRegistryAddress(pointer);
    int size;
    fread(&size, sizeof(int), 1, registryFile);
    pointToRegistryAddress(pointer);
    fread(buffer->data, size, 1, registryFile);
    fwrite(buffer->data, size, 1, newRegistryFile);
    pointer += size;
  }
  fclose(registryFile);
  fclose(newRegistryFile);
  remove("Registry.txt");
  rename("Registry_Temp.txt","Registry.txt");
  registryFile = fopen("Registry.txt", "r+b");
}