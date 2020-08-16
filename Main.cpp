#include <cstring>
#include <iostream>
#include <stdio.h>
#include <string>
#include <iomanip>

#include "Registro.h"
#include "ArvoreB.h"

using namespace std;

void checkFragment(){
   Header* header = readHeader();
   float percentange = (float) (100*header->empty_bytes/header->occupied_bytes);
   if(percentange > 20){
     cout << "ELIMINANDO FRAGMENTACAO, FAVOR AGUARDAR" << "\n";
     defragment();
     cout << "OPERACAO CONCLUIDA COM SUCESSO" << "\n";
   }
   closeFiles();
}

void fragment() {
  openFiles();
  Header* header = readHeader();
  float percentange = (float) header->empty_bytes/header->occupied_bytes;
  cout << "BYTES OCUPADOS: " << header->occupied_bytes 
    << " BYTES LIVRES: " << header->empty_bytes << " FRAGMENTACAO: "
    << fixed << setprecision(2) << percentange*100 << "%\n"; 
  closeFiles();
}

string convertBirthdate(string birthdate) {
  string result = "          ";
  result.at(0) = birthdate.at(6);
  result.at(1) = birthdate.at(7);
  result.at(2) = '/';
  result.at(3) = birthdate.at(4);
  result.at(4) = birthdate.at(5);
  result.at(5) = '/';
  result.at(6) = birthdate.at(0);
  result.at(7) = birthdate.at(1);
  result.at(8) = birthdate.at(2);
  result.at(9) = birthdate.at(3);
  return result;
}

void insert(string commandLine, int posChar) {
  openFiles();
  Registry reg = readRegistryFromCommand(commandLine, posChar);
  writeRegistry(reg);
  cout << "NOVO REGISTRO CRIADO COM SUCESSO" << "\n";
  checkFragment();
}

void search(string commandLine, int posChar) {
  openFiles();
  long long int cpf = readCpfFromCommand(commandLine, posChar);
  Registry reg = readRegistry(cpf);
  if(!reg.vazio){
    string convertedBirthdate = convertBirthdate(to_string(reg.data_nascimento));
    cout << "CPF = " << reg.cpf << "\n"
         << "NOME = " << reg.nome << "\n"
         << "SOBRENOME = " << reg.sobrenome << "\n"
         << "CURSO = " << reg.curso << "\n"
         << "SEXO = " << reg.sexo << "\n"
         << "DATA_NASCIMENTO = " << convertedBirthdate << "\n";
  } else {
    cout << "NENHUM REGISTRO ENCONTRADO" << "\n";
  }
}

void remove(string commandLine, int posChar) {
  openFiles();
  long long int cpf = readCpfFromCommand(commandLine, posChar);
  removeRegistry(cpf);
  cout << "REGISTRO REMOVIDO COM SUCESSO" << "\n";
  checkFragment();
}

void update(string commandLine, int posChar) {
  openFiles();
  Registry reg = readRegistryFromCommand(commandLine, posChar);
  updateRegistry(reg);
  cout << "REGISTRO ATUALIZADO COM SUCESSO" << "\n";
  checkFragment();
}

void close() {
  cout << "ENCERRANDO" << "\n";
  exit(0);
}

string getCommand(string commandLine, int* posChar){
  string command = "";
  for(int i = 0; i < (int) commandLine.length(); i++){
      if(commandLine[i] == '('){
        *posChar = i; 
        break;
      } else {
        command += commandLine[i];
      }
    }
  return command;
}

void checkCommand(string command, string commandLine, int posChar){
  if(command == "INSERE") {
    insert(commandLine, posChar);
  } else if(command == "BUSCA") {
    search(commandLine, posChar);
  } else if(command == "REMOVE") {
    remove(commandLine, posChar);
  } else if(command == "ATUALIZA") {
    update(commandLine, posChar);
  } else if(command == "IMPRIME_FRAGMENTACAO") {
    fragment();
  } else if (command == "FECHA") {
    close();
  }
}

int main(){
  getRootIndex();
  string commandLine;
  int posChar = 0;
  while(getline(cin, commandLine)) {
    string command = getCommand(commandLine, &posChar);
    checkCommand(command, commandLine, posChar);
  }
  freeRoot();
}
