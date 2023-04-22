#include <iostream>
#include <fstream>
using namespace std;

enum class States { Normal, Slash, CommentMulti, Star, CommentSingle, ConstString, BackSlashStr, ConstSymbol, BackSlashSymb, Num, FrstDot, Dot, Exp, AfterExp, NotExp, Float, Long, Z};

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
					else if (symb == '\"')
						State = States::ConstString;
					else if (symb == '\'')
						State = States::ConstSymbol;
					else if (isdigit(symb)) {
						fout << symb;
						State = States::Num;
					}
					else if (symb == '.') {
						fout << symb;
						State = States::FrstDot;
					}
					else if (isalpha(symb) || symb == '.' || symb == '_')
						State = States::Z;
					break;
				case States::Num:
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
					else if (isdigit(symb))
						fout << symb;
					else if (symb == '.') {
						fout << symb;
						State = States::Dot;
					}
					else if (symb == 'e' || symb == 'E') {
						fout << symb;
						State = States::Exp;
					}
					else if (isalpha(symb) || symb == '.' || symb == '_') {
						fout << symb << " Error" << endl;
						State = States::Z;
					}
					break;
				case States::FrstDot:
					if (isdigit(symb)) {
						fout << symb;
						State = States::Dot;
					}
					else if (isalpha(symb) || symb == '.' || symb == '_') {
						fout << symb << " Error" << endl;
						State = States::Z;
					}
					break;
				case States::Dot:
					if (symb == '/') {
						fout << " double" << endl;
						State = States::Slash;
					}
					else if (symb == '\"') {
						fout << " double" << endl;
						fout << symb;
						State = States::ConstString;
					}
					else if (symb == '\'') {
						fout << " double" << endl;
						fout << symb;
						State = States::ConstSymbol;
					}
					else if (isdigit(symb))
						fout << symb;
					else if (symb == 'e' || symb == 'E') {
						fout << symb;
						State = States::Exp;
					}
					else if (symb == 'f' || symb == 'F') {
						fout << symb;
						State = States::Float;
					}
					else if (symb == 'l' || symb == 'L') {
						fout << symb;
						State = States::Long;
					}
					else if (isalpha(symb) || symb == '.' || symb == '_') {
						fout << symb << " Error" << endl;
						State = States::Z;
					}
					else {
						State = States::Normal;
						fout << " double" << endl;
					}
					break;
				case States::Exp:
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
					else if (symb == '+' || symb == '-') {
						fout << symb;
						State = States::AfterExp;
					}
					else if (isdigit(symb)) {
						fout << symb;
						State = States::NotExp;
					}
					else if (isalpha(symb) || symb == '.' || symb == '_') {
						fout << symb << " Error" << endl;
						State = States::Z;
					}
					else {
						fout << " Error" << endl;
						State = States::Normal;
					}
					break;
				case States::AfterExp:
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
					else if (isdigit(symb)) {
						fout << symb;
						State = States::NotExp;
					}
					else if (isalpha(symb) || symb == '.' || symb == '_') {
						fout << symb << " Error" << endl;
						State = States::Z;
					}
					else {
						fout << " Error" << endl;
						State = States::Normal;
					}
					break;
				case States::NotExp:
					if (symb == '/') {
						fout << " double" << endl;
						State = States::Slash;
					}
					else if (symb == '\"') {
						fout << " double" << endl;
						fout << symb;
						State = States::ConstString;
					}
					else if (symb == '\'') {
						fout << " double" << endl;
						fout << symb;
						State = States::ConstSymbol;
					}
					else if (isdigit(symb))
						fout << symb;
					else if (symb == 'f' || symb == 'F') {
						fout << symb;
						State = States::Float;
					}
					else if (symb == 'l' || symb == 'L') {
						fout << symb;
						State = States::Long;
					}
					else if (isalpha(symb) || symb == '.' || symb == '_') {
						fout << symb << " Error" << endl;
						State = States::Z;
					}
					else {
						State = States::Normal;
						fout << " double" << endl;
					}
					break;
				case States::Float:
					if (symb == '/') {
						fout << " float" << endl;
						State = States::Slash;
					}
					else if (symb == '\"') {
						fout << " float" << endl;
						fout << symb;
						State = States::ConstString;
					}
					else if (symb == '\'') {
						fout << " float" << endl;
						fout << symb;
						State = States::ConstSymbol;
					}
					else if (isalpha(symb) || symb == '.' || symb == '_' || isdigit(symb)) {
						fout << symb << " Error" << endl;
						State = States::Z;
					}
					else {
						State = States::Normal;
						fout << " float" << endl;
					}
					break;
				case States::Long:
					if (symb == '/') {
						fout << " long" << endl;
						State = States::Slash;
					}
					else if (symb == '\"') {
						fout << " long" << endl;
						fout << symb;
						State = States::ConstString;
					}
					else if (symb == '\'') {
						fout << " long" << endl;
						fout << symb;
						State = States::ConstSymbol;
					}
					else if (isalpha(symb) || symb == '.' || symb == '_' || isdigit(symb)) {
						fout << symb << " Error" << endl;
						State = States::Z;
					}
					else {
						State = States::Normal;
						fout << " long" << endl;
					}
					break;
				case States::Z:
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
					else if (isalpha(symb) || symb == '.' || symb == '_' || isdigit(symb))
						State = States::Z;
					else
						State = States::Normal;
					break;
				case States::Slash:
					if (symb == '*')
						State = States::CommentMulti;
					else if (symb == '/')
						State = States::CommentSingle;
					else if (isdigit(symb)) {
						fout << symb;
						State = States::Num;
					}
					else if (isalpha(symb) || symb == '.' || symb == '_') {
						fout << symb << " Error" << endl;
						State = States::Z;
					}
					else 
						State = States::Normal;
					break;
				case States::CommentMulti:
					if (symb == '*')
						State = States::Star;
					break;
				case States::CommentSingle:
					if (symb == '\n' || symb == '\r') 
						State = States::Normal;
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
					if (symb == '\\')
						State = States::BackSlashStr;
					else if (symb == '\"')
						State = States::Normal;
					break;
				case States::ConstSymbol:
					if (symb == '\\')
						State = States::BackSlashSymb;
					else if (symb == '\'') 
						State = States::Normal;
					break;
				case States::BackSlashStr:
					State = States::ConstString;
					break;
				case States::BackSlashSymb:
					State = States::ConstSymbol;
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
