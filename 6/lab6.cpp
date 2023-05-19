#include <string>
#include <vector>
#include <fstream>
#include <iostream>
using namespace std;

ifstream fin;

enum class typeErrors {
	SYNTAX_ERR,
	UNDEF_ID,
	ID_MISS,
	SYMBOL_MISS,
	NO_VARIABLES,
	UNKNOWN_SYMBOL
};

static void Error(typeErrors type, const string param);

struct lexeme
{
	string type;
	string value;
	lexeme() { type = ""; value = ""; }
};

class handler
{
private:
	//vector <lexeme> lexemes;
	char symb = EOF;
	lexeme lex;

	const string GrammerSymbols = "=+-/*();";
public:
	inline void GetSymbol(void);
	void Scanner(void);
	void Run(void);
};

static void Error(typeErrors type, const string param = "") {
	switch (type)
	{
	case typeErrors::SYNTAX_ERR:
		cout << "Error: Syntax error." << endl;
		break;
	case typeErrors::UNDEF_ID:
		cout << "Error: Undefined identifier. " + param << endl;
		break;
	case typeErrors::ID_MISS:
		cout << "Error: Identifier missing." + param << endl;
		break;
	case typeErrors::SYMBOL_MISS:
		cout << "Error: Symbol missing. \'" + param + "\'" << endl;
		break;
	case typeErrors::NO_VARIABLES:
		cout << "Error: No variables defined yet.f" + param << endl;
	case typeErrors::UNKNOWN_SYMBOL:
		cout << "Error: Unknown symbol " + param << endl;
	default:
		break;
	}
	fin.close();
	exit(1);
}

inline void handler::GetSymbol(void)
{
	fin.get(symb);
}

void handler::Scanner(void)
{
	if (isalpha(symb) or symb == '_')
	{
		GetSymbol();
		while (isalnum(symb) || symb == '_')
		{
			lex.value += symb;
			GetSymbol();
		}
		lex.type = "I";
	}
	else if (isdigit(symb))
	{
		GetSymbol();
		while (isdigit(symb))
		{
			lex.value += symb;
			GetSymbol();
		}
		lex.type = "C";
	}
	else if (GrammerSymbols.find(symb) != string::npos)
	{
		lex.type = symb;
		lex.value = symb;
		GetSymbol();
	}
	else
		Error(typeErrors::UNKNOWN_SYMBOL, string(1,symb));
}

void handler::Run(void)
{
	GetSymbol();
	while(true)
	{
		while (symb == ' ' or symb == '\n')
			GetSymbol();
		Scanner();
		cout << lex.type;
		if (lex.type == ";")
			cout << endl;
		if (fin.eof())
			break;
	}
}

int main(void) {
	fin.open("Source.txt");
	//fout.open("Output.txt");
	if (fin.is_open() /*&& fout.is_open()*/) {
		handler begin;
		begin.Run();
	}
	else {
		cout << "\nInput file \'Source.txt\' open error." << endl;
		return 1;
	}
	fin.close();
	//fout.close();
	return 0;
}
