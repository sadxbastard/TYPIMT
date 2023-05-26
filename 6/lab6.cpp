#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#define SIZE_MATRIX_RELATION 18
#define SIZE_MATRIX_RULES 13

using namespace std;

ifstream fin;
ofstream fout;
int IdTriad = 0;

enum class typeErrors {
	SYNTAX_ERR,
	UNDEF_ID,
	ID_MISS,
	SYMBOL_MISS,
	NO_VARIABLES,
	UNKNOWN_SYMBOL
};

static void Error(typeErrors type, const string param);

struct var {
	string name;
	long value;
	var() : name(""), value(numeric_limits<long>::min()) {}
};

struct triad {
	string typeOperation;
	string firstOperand;
	string secondOperand;
	triad(string typeOperation, string firstOperand, string secondOperand);
};

triad::triad(string typeOp, string firstOp, string secondOp) {
	this->typeOperation = typeOp;
	this->firstOperand = firstOp;
	this->secondOperand = secondOp;
}

char matrix[SIZE_MATRIX_RELATION][SIZE_MATRIX_RELATION] = {
		{' ', 'L', 'S', 'E', 'T', 'M', '#', ':', '=', ';', '+', '-', '*', '/', '(', ')', 'I', 'C' },
		{'L', ' ', '=', ' ', ' ', ' ', '=', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', ' ' },
		{'S', ' ', ' ', ' ', ' ', ' ', '>', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', ' ' },
		{'E', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '=', '=', '=', ' ', ' ', ' ', '=', ' ', ' ' },
		{'T', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '=', '=', ' ', '>', ' ', ' ' },
		{'M', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>', ' ', '>', ' ', ' ' },
		{'#', '%', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', ' ' },
		{':', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '=', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ' },
		{'=', ' ', ' ', '%', '<', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', ' ', '<', '<' },
		{';', ' ', ' ', ' ', ' ', ' ', '>', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', ' ' },
		{'+', ' ', ' ', ' ', '%', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', ' ', '<', '<' },
		{'-', ' ', ' ', ' ', '%', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', ' ', '<', '<' },
		{'*', ' ', ' ', ' ', ' ', '=', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', ' ', '<', '<' },
		{'/', ' ', ' ', ' ', ' ', '=', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', ' ', '<', '<' },
		{'(', ' ', ' ', '%', '<', '<', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '<', ' ', '<', '<' },
		{')', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>', ' ', '>', ' ', ' ' },
		{'I', ' ', ' ', ' ', ' ', ' ', ' ', '=', ' ', '>', '>', '>', '>', '>', ' ', '>', ' ', ' ' },
		{'C', ' ', ' ', ' ', ' ', ' ', ' ', ' ', ' ', '>', '>', '>', '>', '>', ' ', '>', ' ', ' ' }
};

class rule
{
public:
	char left;
	string right;
	rule(char _left, string _right);
};

rule::rule(char _left, string _right)
{
	left = _left;
	right = _right;
}

rule rules[SIZE_MATRIX_RULES] = {
	{'Z', "#L#"},
	{'L', "LS"}, {'L', "S"},
	{'S', "I:=E;"},
	{'E', "E+T"}, {'E', "E-T"}, {'E', "T"},
	{'T', "T*M"}, {'T', "T/M"}, {'T', "M"},
	{'M', "(E)"}, {'M', "I"}, {'M', "C"}
};

struct lexeme
{
	char type;
	string value;
	char typeRelation;
	int idTriad;
	lexeme() { type = '\0'; value = ""; }
};

class analyzer
{
private:
	vector <var> vars;
	vector <triad> triads;
	vector <lexeme> stack;

	char symb = EOF;
	lexeme lex;

	const string GrammerSymbols = ":=+-/*();";
	
public:
	inline void GetSymbol(void);
	inline void GetSymbolExcludingSpaces(void);

	char FindRelationMatrix(char Y);
	char FindRule(string ruleRight);
	int FindRelationStack(char symbol);

	void Scanner(void);
	void Parser(void);
	char Convolution(void);
	void CreateTriad(string base, int indexLeft);
	bool VarValue(string name);
	bool VarPointer(string name);

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

inline void analyzer::GetSymbol(void)
{
	fin.get(symb);
}

inline void analyzer::GetSymbolExcludingSpaces(void)
{
	while (isspace(symb)) GetSymbol();
}

char analyzer::FindRelationMatrix(char Y)
{
	char X = stack.back().type;
	int i = 0, j = 0;
	try
	{
		while (matrix[i][0] != X)
			i++;
		while (matrix[0][j] != Y)
			j++;
	}
	catch(exception ex)
	{
		Error(typeErrors::UNKNOWN_SYMBOL, "No element found in the matrix");
	}
	return matrix[i][j];
}

char analyzer::FindRule(string ruleRight)
{
	for (int i = 0; i <= SIZE_MATRIX_RULES; i++)
		if (rules[i].right == ruleRight)
			return rules[i].left;
	return NULL;
}

int analyzer::FindRelationStack(char symbol) // разобраться
{
	for (int i = stack.size() - 1; i > 0; i--)
		if (stack[i].typeRelation == symbol)
			return i;
	return 0;
}

void analyzer::Scanner(void)
{
	GetSymbolExcludingSpaces();
	lex.value = "";
	if (isalpha(symb) or symb == '_')
	{
		while (isalnum(symb) || symb == '_')
		{
			lex.value += symb;
			GetSymbol();
		}
		lex.type = 'I';
	}
	else if (isdigit(symb))
	{
		while (isdigit(symb))
		{
			lex.value += symb;
			GetSymbol();
		}
		lex.type = 'C';
	}
	else if (GrammerSymbols.find(symb) != string::npos)
	{
		lex.type = symb;
		GetSymbol();
	}
	else if (symb == EOF)
		lex.type = '#';
	else
		Error(typeErrors::UNKNOWN_SYMBOL, string(1, symb));
}

void analyzer::Parser()
{
	char token = lex.type;
	char relation = FindRelationMatrix(lex.type);
	if (relation == ' ')
		Error(typeErrors::UNKNOWN_SYMBOL, "No relation for the lexeme");
	else if (relation != '>')
	{
		lex.typeRelation = relation;
		if(lex.type == 'C')
		{
			triad triadVar("C", lex.value, "@");
			triads.push_back(triadVar);
			IdTriad++;
			lex.idTriad = IdTriad;
		}
		else if(lex.type == 'I')
		{
			triad triadVar("V", lex.value, "@");
			triads.push_back(triadVar);
			IdTriad++;
			lex.idTriad = IdTriad;
		}
		stack.push_back(lex);
	}
	else
	{
		while(relation == '>')
		{
			lex.type = Convolution();
			lex.typeRelation = FindRelationMatrix(lex.type);
			lex.idTriad = IdTriad;
			stack.push_back(lex);
			relation = FindRelationMatrix(token);
		}
		lex.type = token;
		lex.typeRelation = relation;
		if(lex.type == 'I')
		{
			triad triadVar("V", lex.value, "@");
			triads.push_back(triadVar);
			IdTriad++;
			lex.idTriad = IdTriad;
		}
		stack.push_back(lex);
	}
}

char analyzer::Convolution(void)
{
	string base;
	int indexLeft, temp = 0;
	char leftRule;
	bool flag = false;

	indexLeft = FindRelationStack('<');
	if (indexLeft == 0)
		indexLeft = FindRelationStack('%');

	while(true)
	{
		for (temp = indexLeft; temp < stack.size(); temp++)
			base += stack[temp].type;
		CreateTriad(base, indexLeft);
		if(leftRule = FindRule(base))
		{
			for (; indexLeft < stack.size();)
				stack.pop_back();
			return leftRule;
		}
		else
		{
			base.clear();
			for (; indexLeft < stack.size(); indexLeft++)
			{
				if (stack[indexLeft].typeRelation == '%' && !flag)
				{
					flag = true;
					break;
				}
				else
					flag = false;
			}
			if (indexLeft == stack.size())
				Error(typeErrors::SYNTAX_ERR);
		}
	}
}

void analyzer::CreateTriad(string base, int indexLeft)
{
	if (base == "I:=E;")
	{
		VarPointer(stack[indexLeft].value);
		triad triadVar(":=", '^' + to_string(stack[indexLeft].idTriad), '^' + to_string(stack[indexLeft + 3].idTriad));
		triads.push_back(triadVar);
		IdTriad++;
	}
	else if (base == "E+T")
	{
		triad triadVar("+", '^' + to_string(stack[indexLeft].idTriad), '^' + to_string(stack[indexLeft + 2].idTriad));
		triads.push_back(triadVar);
		IdTriad++;
	}
	else if (base == "E-T")
	{
		triad triadVar("-", '^' + to_string(stack[indexLeft].idTriad), '^' + to_string(stack[indexLeft + 2].idTriad));
		triads.push_back(triadVar);
		IdTriad++;
	}
	else if (base == "T*M")
	{
		triad triadVar("*", '^' + to_string(stack[indexLeft].idTriad), '^' + to_string(stack[indexLeft + 2].idTriad));
		triads.push_back(triadVar);
		IdTriad++;
	}
	else if (base == "T/M")
	{
		triad triadVar("/", '^' + to_string(stack[indexLeft].idTriad), '^' + to_string(stack[indexLeft + 2].idTriad));
		triads.push_back(triadVar);
		IdTriad++;
	}
	else if (base == "(E)")
		IdTriad = stack[indexLeft + 1].idTriad;
	else if (base == "I")
		VarValue(stack[indexLeft].value);
}

bool analyzer::VarValue(string name)
{
	if (name.empty() or name.size() == 0)
		Error(typeErrors::SYNTAX_ERR);
	for (int i = 0; i < vars.size(); i++)
		if (vars[i].name == name)
			return true;
	Error(typeErrors::ID_MISS, name);
}

bool analyzer::VarPointer(string name)
{
	if (name.empty() or name.size() == 0)
		Error(typeErrors::SYNTAX_ERR);
	for (int i = 0; i < vars.size(); i++)
		if (vars[i].name == name)
			return true;
	var temp_var;
	temp_var.name = name;
	vars.push_back(temp_var);
	return true;
}



void analyzer::Run(void)
{
	lexeme temp_lex;
	temp_lex.type = '#';
	temp_lex.typeRelation = ' ';
	stack.push_back(temp_lex);
	GetSymbol();
	while (true)
	{
		Scanner();
		Parser();
		if (fin.eof())
			break;
	}
	for (int i = 0; i < triads.size(); i++)
		fout << i + 1 << ": " << triads[i].typeOperation << '(' << triads[i].firstOperand << ", " << triads[i].secondOperand << ')' << endl;
	cout << "Successful execution of the program" << endl;
}

int main(void) {
	fin.open("Source.txt");
	fout.open("Output.txt");
	if (fin.is_open() && fout.is_open()) {
		analyzer begin;
		begin.Run();
	}
	else {
		cout << "\nInput file \'Source.txt\' open error." << endl;
		return 1;
	}
	fin.close();
	fout.close();
	return 0;
}
