#ifndef __ROOT
#define __ROOT
#include <iostream>
#include <ctime>
using namespace std;

class RootDirectory{
	private :
		string namaFile;
		char fileAtt;
		unsigned short clock;
		unsigned short date;
		unsigned short dir;
		int size;
		
	public : 
		RootDirectory();
		RootDirectory(string name, char att, short _dir, int _size);
		void setAtt(char att);
		void setClock();
		void setDate();
		void setDir(short _dir);
		void setSize(int _size);
		string DecToBin(int number);
		int BinToDec(string number);
		string checkBin(string binary,string status);
		string toString();
		void load(string);
};
#endif
