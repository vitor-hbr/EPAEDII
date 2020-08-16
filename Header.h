using namespace std;

#include "FileHandler.h"

typedef struct Header {
  int occupied_bytes;
  int empty_bytes;
  int empty_list_address;
} Header;

void writeHeader(Header* header);
Header* readHeader();
int headerSize();