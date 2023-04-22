#include <iostream>
#include <fstream>
using namespace std;

enum class States { Normal, Slash, Comment, Star };

int main() {
	char symb;
	ifstream fin;
	ofstream fout;
	States State = States::Normal;
	fin.open("Source.c");
	if (fin.is_open()) {
		fout.open("Out.c");
		if (fout.is_open()) {
			while (fin.get(symb)) {
				switch (State) {
				case States::Normal:
					if (symb == '/')
						State = States::Slash;
					else
						fout << symb;
					break;
				case States::Slash:
					if (symb == '*')
						State = States::Comment;
					else if (symb == '/') {
						fout << '/';
						break;
					}
					else {
						State = States::Normal;
						fout << '/' << symb;
					}
					break;
				case States::Comment:
					if (symb == '*')
						State = States::Star;
					break;
				case States::Star:
					if (symb == '/') {
						fout << ' ';
						State = States::Normal;
					}
					else if (symb == '*')
						break;
					else
						State = States::Comment;
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
