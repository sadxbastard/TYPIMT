#include <iostream>
#include <fstream>
using namespace std;

enum class States { Normal, Slash, CommentMulti, Star, CommentSingle, ConstString, BackSlashStr, ConstSymbol, BackSlashSymb};

int main() {
	char symb;
	ifstream fin;
	ofstream fout;
	States State = States::Normal;
	fin.open("Source.txt");
	if (fin.is_open()) {
		fout.open("Out.txt");
		if (fout.is_open()) {
			while (fin.get(symb)) {
				switch (State) {
				case States::Normal:
					if (symb == '/')
						State = States::Slash;
					else if (symb == '\"') {
						fout << symb;
						State = States::ConstString;
					}
					else if (symb == '\'') {
						fout << symb;
						State = States::ConstSymbol;
					}
					else fout << symb;
					break;
				case States::Slash:
					if (symb == '*')
						State = States::CommentMulti;
					else if (symb == '/')
						State = States::CommentSingle;
					else {
						fout << '/' << symb;
						State = States::Normal;
					}
					break;
				case States::CommentMulti:
					if (symb == '*')
						State = States::Star;
					break;
				case States::CommentSingle:
					if (symb == '\n' || symb == '\r') {
						fout << symb;
						State = States::Normal;
					}
					break;
				case States::Star:
					if (symb == '/')
						State = States::Normal;
					else if (symb == '*')
						break;
					else
						State = States::CommentMulti;
					break;
				case States::ConstString:
					if (symb == '\\') {
						fout << symb;
						State = States::BackSlashStr;
					}
					else if (symb == '\"') {
						fout << symb;
						State = States::Normal;
					}
					else fout << symb;
					break;
				case States::ConstSymbol:
					if (symb == '\\') {
						fout << symb;
						State = States::BackSlashSymb;
					}
					else if (symb == '\'') {
						fout << symb;
						State = States::Normal;
					}
					else fout << symb;
					break;
				case States::BackSlashStr:
					fout << symb;
					State = States::ConstString;
					break;
				case States::BackSlashSymb:
					fout << symb;
					State = States::BackSlashSymb;
					break;
				}
			}
		}
		else {
			fin.close();
			cout << "Output file /Out.cpp/ open error." << endl;
			return 2;
		}
	}
	else {
		cout << "Input file /Source.cpp/ open error." << endl;
		return 1;
	}
	fin.close();
	fout.close();
	return 0;
}
