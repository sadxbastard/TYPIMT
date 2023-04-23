#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <limits>
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

ifstream fin;
ofstream fout;
ofstream fOptimization;
int IdTriad = 0;

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

// Стурктура для хранения параметров переменной
struct var {
	string name;
	long value;
	var() : name(""), value(numeric_limits<long>::min()) {}
};

struct triad {
	string typeOperation;
	string firstOperand;
	string secondOperand;
	bool isDeleted = false;
	triad(string typeOperation, string firstOperand, string secondOperand);
};

// Класс для обработки входного файла
class handler {
private:
	vector <var> vars; // вектор для хранения переменных
	vector <triad> triads; // вектор для хранения триад
	char symb = EOF;
public:
	inline void Get(void);               // считать следующий символ
	long VarPointer(const string name);  // получение адреса переменной
	long VarValue(const string name);	 // получение значения переменной
	void Run(void);                      // запуск программы
	void Optimization(void);

	void ProcS(void);
	long ProcE(void);
	long ProcT(void);
	long ProcM(void);
	long ProcI(void);
	long ProcC(void);
	long ProcL(void);
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

long handler::VarPointer(const string name) {
	if (name.empty())
		Error(typeErrors::SYNTAX_ERR);
	for (int i = 0; i < vars.size(); i++) {
		if (vars[i].name == name) { // проверка на наличие идентификатора в векторе
			IdTriad++;
			triad obj("V", name, "@");
			triads.push_back(obj);
			return IdTriad;
		}
	}
	var temp_var;
	temp_var.name = name;
	vars.push_back(temp_var);
	IdTriad++;
	triad obj("V", name, "@");
	triads.push_back(obj);
	return IdTriad;
}

long handler::VarValue(const string name) {
	if (name.empty())
		Error(typeErrors::SYNTAX_ERR);
	for (int i = 0; i < vars.size(); i++) {
		if (vars[i].name == name) {
			IdTriad++;
			triad obj("V", name, "@");
			triads.push_back(obj);
			return IdTriad;
		}
	}
	Error(typeErrors::UNDEF_ID, name);
}


inline void handler::Get(void) {
	fin.get(symb);
}

// Уровни III-VI

long handler::ProcC(void) {
	string x;
	while (isdigit(symb)) {
		x += symb;
		Get();
	}
	IdTriad++;
	triad obj("C", x, "@");
	triads.push_back(obj);
	return IdTriad;
}

long handler::ProcI(void) {
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

long handler::ProcL(void) {
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

long handler::ProcM(void) { 
	long x;
	if (symb == '(') {
		Get();
		while (symb == ' ')
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
	while (symb == ' ')
		Get();
	return x;
}

long handler::ProcT(void) { 
	long x = ProcM();
	long tempX;
	while (symb == '*' || symb == '/') {
		char temp_symb = symb;
		Get();
		while (symb == ' ')
			Get();
		if (temp_symb == '*') {
			tempX = ProcM();
			IdTriad++;
			triad obj("*", '^' + to_string(x), '^' + to_string(tempX));
			triads.push_back(obj);
			x = IdTriad;
		}
		else {
			tempX = ProcM();
			IdTriad++;
			triad obj("/", '^' + to_string(x), '^' + to_string(tempX));
			triads.push_back(obj);
			x = IdTriad;
		}
	}
	return x;
}

long handler::ProcE(void) {
	long x = ProcT();
	long tempX;
	while (symb == '+' || symb == '-') {
		char temp_symb = symb;
		Get();
		while (symb == ' ')
			Get();
		if (temp_symb == '+') {
			tempX = ProcT();
			IdTriad++;
			triad obj("+", '^' + to_string(x), '^' + to_string(tempX));
			triads.push_back(obj);
			x = IdTriad;
		}
		else {
			tempX = ProcT();
			IdTriad++;
			triad obj("-", '^' + to_string(x), '^' + to_string(tempX));
			triads.push_back(obj);
			x = IdTriad;
		}
	}
	return x;
}

// Уровень I

void handler::ProcS(void) {
	long triad1 = ProcL();
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
	long triad2 = ProcE();
	while (symb == ' ')
		Get();
	if (symb != ';')
		Error(typeErrors::SYMBOL_MISS, ";");
	IdTriad++;
	triad obj(":=", '^' + to_string(triad1), '^' + to_string(triad2));
	triads.push_back(obj);
	Get();
}

// Высший уровень

void handler::Run(void) {
	Get();
	cout << "Begin parsing." << endl;
	while(true) {
		while (symb == ' ' || symb == '\n')
			Get();
		if (fin.eof())
			break;
		ProcS();
	}
	cout << "Successful completion of parsing." << endl;
	for (int i = 0; i < triads.size(); i++)
		fout << i + 1 << ": " << triads[i].typeOperation << '(' << triads[i].firstOperand << ", " << triads[i].secondOperand << ')' << endl;
	Optimization();
	for (int i = 0; i < triads.size(); i++) {
		if (triads[i].isDeleted == false)
			fOptimization << i + 1 << ": " << triads[i].typeOperation << '(' << triads[i].firstOperand << ", " << triads[i].secondOperand << ')' << endl;
		else
			fOptimization << i + 1 << ":" << endl;
	}
}

void handler::Optimization(void) {
	for (int i = 0; i < triads.size(); i++) {
		bool flag = 1;
		while (flag) {
			if (triads[i].typeOperation != ":=" && triads[i].secondOperand[0] == '^') {
				int tempIdTriad = stoi(triads[i].secondOperand.substr(1)) - 1; // получение номера триады, путем извлечения подстроки и преобразования ее в int
				if (triads[tempIdTriad].typeOperation == "C") {
					triads[i].secondOperand = triads[tempIdTriad].firstOperand;
					triads[tempIdTriad].isDeleted = true;
				}
				else flag = 0;
			}
			else if (triads[i].typeOperation != ":=" && triads[i].firstOperand[0] == '^' && isdigit(triads[i].secondOperand[0])) {
				int tempIdTriad = stoi(triads[i].firstOperand.substr(1)) - 1;
				if (triads[tempIdTriad].typeOperation == "C") {
					long result;
					if (triads[i].typeOperation == "+")
						result = stol(triads[tempIdTriad].firstOperand) + stol(triads[i].secondOperand);
					else if (triads[i].typeOperation == "-")
						result = stol(triads[tempIdTriad].firstOperand) - stol(triads[i].secondOperand);
					else if (triads[i].typeOperation == "*")
						result = stol(triads[tempIdTriad].firstOperand) * stol(triads[i].secondOperand);
					else
						result = stol(triads[tempIdTriad].firstOperand) / stol(triads[i].secondOperand);
					triads[i].firstOperand = to_string(result);
					triads[i].secondOperand = '@';
					triads[i].typeOperation = "C";
					triads[tempIdTriad].isDeleted = true;
				}
				else flag = 0;
			}
			else if (triads[i].typeOperation == ":=" && triads[i].firstOperand[0] == '^') {
				int tempIdTriad = stoi(triads[i].firstOperand.substr(1)) - 1;
				if (triads[tempIdTriad].firstOperand[0] != '^' && isdigit(triads[tempIdTriad].firstOperand[0]) == 0) {
					triads[i].firstOperand = triads[tempIdTriad].firstOperand;
					triads[tempIdTriad].isDeleted = true;
				}
				else flag = 0;
			}
			else flag = 0;
		}
		
	}
}

triad::triad(string typeOp, string firstOp, string secondOp) {
	this->typeOperation = typeOp;
	this->firstOperand = firstOp;
	this->secondOperand = secondOp;
}

int main(void) {
	fin.open("Source.txt");
	fout.open("Output.txt");
	fOptimization.open("Optimization.txt");
	if (fin.is_open() && fout.is_open()) {
		handler begin;
		begin.Run();
	}
	else {
		cout << "\nInput file \'Source.txt\' or \'Output.txt\' open error." << endl;
		return 1;
	}
	fin.close();
	fout.close();
	fOptimization.close();
	return 0;
}
