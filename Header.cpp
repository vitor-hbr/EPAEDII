#include <cstring>
#include <iostream>
#include <stdio.h>

#include "Registro.h"

using namespace std;

void writeHeader(Header* header) {
  pointToStartRegistryFile();
  Buffer* buffer = (Buffer*) malloc(sizeof(Buffer));
  buffer->prox = 0;
  writeIntToBuffer(buffer, header->occupied_bytes);
  writeIntToBuffer(buffer, header->empty_bytes);
  writeIntToBuffer(buffer, header->empty_list_address);
  writeBlock(buffer->data,headerSize(),false);
}

Header* readHeader() {
  pointToStartRegistryFile();
  Buffer* buffer = (Buffer*) malloc(sizeof(Buffer));
  readBlock(buffer, headerSize(), false);
  Header* header = (Header*) malloc(sizeof(Header));
  header->occupied_bytes = readIntFromBuffer(buffer);
  header->empty_bytes = readIntFromBuffer(buffer);
  header->empty_list_address = readIntFromBuffer(buffer);
  free(buffer);
  return header;
}

int headerSize() {
  return sizeof(int) * 3;
}