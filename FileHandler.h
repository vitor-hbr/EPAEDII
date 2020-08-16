#ifndef MY_HEADER_H
#define MY_HEADER_H

#define PIPE '|'
#define BUFFER_SIZE 500
#include <string>
#include <cstring>
#include <iostream>
#include <stdio.h>

typedef struct Buffer {
   unsigned char data[BUFFER_SIZE];
   int prox;
   bool folha;
} Buffer;

using namespace std;

bool checkIfHasEmptySpace(int address);
int getAddressEndOfRegistryFile();
int getAddressEndOfIndexFile();
void pointToStartRegistryFile();
void pointToStartIndexFile();
void pointToEndRegistryFile();
void pointToEndIndexFile();
void pointToRegistryAddress(int address);
void pointToIndexAddress(int address);
void writeBlock(void *  data, int numOfBytes, bool isIndexFile);
void writeStringToBuffer(Buffer* buffer, string string);
void writeInt(int n, int intSize, bool isIndexFile);
void writeIntToBuffer(Buffer* buffer, long long int number);
void writeLongLongIntToBuffer(Buffer* buffer,  long long int number);
void writeSeparatorToBuffer(Buffer* buffer);
void writeCharToBuffer(Buffer* buffer, char c);
void writeBoolToBuffer(Buffer* buffer,  bool b);
void readBlock(Buffer* buffer, int numOfBytes, bool isIndexFile);
Buffer* readRegistryFromFile(int numOfBytes, int address);
Buffer* readNoFromFile(int address);
int readIntFromBuffer(Buffer* buffer);
long long int readLongLongIntFromBuffer(Buffer* buffer);
char readCharFromBuffer(Buffer* buffer);
void readStringFromBuffer(Buffer* buffer, string* s);
bool readBoolFromBuffer(Buffer* buffer);
void readIntArrayFromBuffer(Buffer* buffer, int* filhos, int nChaves);
void openFiles();                                                       
void closeFiles();
void defragment();
bool registryFileExists();
void reWriteRegistryFile();
#endif