#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <limits> // для инициализации конструктора класса var поля типа long значением "ничего"
using namespace std;

/*
Множество правил грамматики
Вариант - 14

  S ::= I:=E;                 Оператор присваивания
  E ::= E+T | E-T | T         Операции + и -
  T ::= T*M | T/M | M         Операции * и /
  M ::= (E) | I | C			  Cкобки, переменная, константа
  I ::= AK | A                Идентификатор: первая буква,
  K ::= AK | DK | A | D          затем буквы и цифры
  C ::= DC | D                Константа: последовательность цифр
  A ::= a | b | ... | z | _   Буквы
  D ::= 0 | 1 | ... | 9       Цифры

*/


// Перечисление ошибок
enum class typeErrors {
	SYNTAX_ERR,
	UNDEF_ID,
	ID_MISS,
	SYMBOL_MISS,
	NO_VARIABLES
};

// Обработка ошибок: вывод сообщения и завершение программы
static void Error(typeErrors type, const string param);

// Класс для хранения параметров переменной
class var {
//private:
public:
	string name;
	long value;
	var() : name(""), value(numeric_limits<long>::min()) {}
	bool isNull() { // метод для проверки поля value на отсутсвие значения "ничего"
		return (this->value == numeric_limits<long>::min());
	}
	bool isNotNamed() { // метод для проверки поля name на отсутсвие пустой строки
		return (this->name == "");
	}
};

ifstream fin;

class Analizer {
private:
	vector <var> vars; // вектор для хранения переменных
	char symb = EOF;
	int last = -1;
public:
	inline void Get(void);               // считать следующий символ
	long* VarPointer(const string name); // получение адреса переменной
	long VarValue(const string name);	 // получение значения переменной
	void VarPrint(void);				 // печать последней переменной
	void VarPrintAll(void);				 // печать всех переменных
	void Run(void);                      // запуск программы

	void ProcS(void);
	long ProcE(void);
	long ProcT(void);
	long ProcM(void);
	long ProcI(void);
	long ProcC(void);
	long* ProcL(void);
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
	default:
		break;
	}
	fin.close();
	exit(1);
}

long* Analizer::VarPointer(const string name) {
	if (name.empty())
		Error(typeErrors::SYNTAX_ERR);
	for (int i = 0; i < vars.size(); i++) {
		if (strcmp(vars[i].name.c_str(), name.c_str()) == 0)
			return &vars[last = i].value;
	}
	var temp_var;
	temp_var.name = name;
	vars.push_back(temp_var);
	last = vars.size() - 1;
	return &vars[vars.size() - 1].value;
}

long Analizer::VarValue(const string name) {
	if (name.empty())
		Error(typeErrors::SYNTAX_ERR);
	for (int i = 0; i < vars.size(); i++) {
		if (strcmp(vars[i].name.c_str(), name.c_str()) == 0)
			return vars[i].value;
	}
	Error(typeErrors::UNDEF_ID, name);
}

void Analizer::VarPrint(void) {
	if (last < 0)
		Error(typeErrors::NO_VARIABLES);
	else
		cout << vars[last].name << " := " << vars[last].value << endl;
}

void Analizer::VarPrintAll(void) {
	if (vars.empty())
		Error(typeErrors::NO_VARIABLES);
	else {
		for (int i = 0; i < vars.size(); i++)
			cout << vars[i].name << " := " << vars[i].value << endl;
	}
}

inline void Analizer::Get(void) {
	fin.get(symb);
}

// Уровни III-VI

long Analizer::ProcC(void) {
	string x;
	while (isdigit(symb)) {
		x += symb;
		Get();
	}
	return stol(x); // преобрзуем строку в тип long
}

long Analizer::ProcI(void) {
	string identifier;
	if (isalpha(symb) || symb == '_') {
		identifier += symb;
		Get();
	}
	else
		Error(typeErrors::ID_MISS);
	while (isalpha(symb) || symb == '_' || isdigit(symb)) {
		identifier += symb;
		Get();
	}
	if (identifier.empty())
		Error(typeErrors::ID_MISS);
	return VarValue(identifier);
}

long* Analizer::ProcL(void) {
	string identifier;
	if (isalpha(symb) || symb == '_') {
		identifier += symb;
		Get();
	}
	else
		Error(typeErrors::ID_MISS);
	while (isalpha(symb) || symb == '_' || isdigit(symb)) {
		identifier += symb;
		Get();
	}
	if (identifier.empty())
		Error(typeErrors::ID_MISS);
	return VarPointer(identifier);
}

// Уровень II

long Analizer::ProcM(void) {
	long x;
	if (symb == '(') {
		Get();
		x = ProcE();
		if (symb != ')')
			Error(typeErrors::SYMBOL_MISS, ")");
		Get();
	}
	else if (isalpha(symb))
		x = ProcI();
	else if (isdigit(symb))
		x = ProcC();
	else
		Error(typeErrors::SYNTAX_ERR);
	return x;
}

long Analizer::ProcT(void) {
	long x = ProcM();
	while (symb == '*' || symb == '/') {
		char temp_symb = symb;
		Get();
		if (temp_symb == '*')
			x *= ProcM();
		else
			x /= ProcM();
	}
	return x;
}

long Analizer::ProcE(void) {
	long x = ProcT();
	while (symb == '+' || symb == '-') {
		char temp_symb = symb;
		Get();
		if (temp_symb == '+')
			x += ProcT();
		else
			x -= ProcT();
	}
	return x;
}

// Уровень I

void Analizer::ProcS(void) {
	long* ptr = ProcL();
	while (symb == ' ')
		Get();
	if (symb != ':')
		Error(typeErrors::SYMBOL_MISS, ":");
	Get();
	if(symb != '=')
		Error(typeErrors::SYMBOL_MISS, "=");
	Get();
	while (symb == ' ')
		Get();
	*ptr = ProcE();
	while (symb == ' ')
		Get();
	if (symb != ';')
		Error(typeErrors::SYMBOL_MISS, ";");
	Get();
}

// Высший уровень

void Analizer::Run(void) {
	int i;
	Get();
	cout << "Begin parsing." << endl;
	for (i = 0; ; i++) {
		while (symb == ' ' || symb == '\n')
			Get();
		if (fin.eof())
			break;
		cout << "Operator " << i + 1 << ":   ";
		ProcS();
		VarPrint();
	}
	cout << "End parsing." << endl;
	if (!vars.empty()) {
		cout << vars.size() << " variables defined in " << i << " operators:" << endl;
	}
	VarPrintAll();
}

int main(void) {
	string nameF = "Source.txt";
	fin.open(nameF);
	if (fin.is_open()) {
		Analizer begin;
		begin.Run();
	}
	else {
		cout << "\nInput file \'" << nameF <<"\' open error." << endl;
		return 1;
	}
	fin.close();
	return 0;
}
