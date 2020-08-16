all: ArvoreB.o Index.o FileHandler.o Registro.o Header.o Main.cpp
	g++ -g -Wall -Wextra -o EP Main.cpp ArvoreB.o Index.o FileHandler.o Registro.o Header.o

ArvoreB.o: ArvoreB.cpp ArvoreB.h
	g++ -c ArvoreB.cpp

Index.o: Index.h Index.cpp
	g++ -c Index.cpp 

FileHandler.o: FileHandler.h FileHandler.cpp
	g++ -c FileHandler.cpp

Registro.o: Registro.h Registro.cpp
	g++ -c Registro.cpp

Header.o: Header.h Header.cpp
	g++ -c Header.cpp