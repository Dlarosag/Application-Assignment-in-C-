/*
 *  UCF COP3330 Fall 2021 Assignment 6 Solution
 *  Copyright 2021 first_name last_name
 */


#include "std_lib_facilities.h"

struct Token {
	char kind;
	double value;
	string name;
	Token(char ch) :kind(ch), value(0) { }
	Token(char ch, double val) :kind(ch), value(val) { }
	Token(char ch, string str) :kind(ch), name(str),value(0) { }
};

class Token_stream {
	bool full;
	Token buffer;
public:
	Token_stream() :full(0), buffer(0) { }

	Token get();
	void unget(Token t) { buffer = t; full = true; }

	void ignore(char);
};

const char let = 'L';
const char quit = 'Q';
const char print = ';';
const char number = '8';
const char name = 'a';

///Reads 1 char and operates in the following way:
/////////////////////////
/////////////////////////    If the input is an operator or a brace: returns a token with the symbol as the token's kind and a value of 0;
/////////////////////////
/////////////////////////	 If the input is a number it reads the whole sequence and returns a token, setting the token's kind as a number, and setting its value to the input value.
/////////////////////////
/////////////////////////	 else its treated as a name or command (Consider cheking)
Token Token_stream::get()
{
	if (full) { full = false; return buffer; }
	char ch;
	cin >> ch;
	switch (ch) {
	case '(':
	case ')':
	case '+':
	case '-':
	case '*':
	case '/':
	case '%':
	case ';':
	case '=':
		return Token(ch);
	case '.':
	case '0':
	case '1':
	case '2':
	case '3':
	case '4':
	case '5':
	case '6':
	case '7':
	case '8':
	case '9':
	{	cin.unget();
	double val;
	cin >> val;
	return Token(number, val);
	}
	default:
		if (isalpha(ch) || ch == '_') {
			string s;
			s += ch;
			while (cin.get(ch) && (isalpha(ch) || isdigit(ch) || ch == '_')) s += ch;
			cin.unget();

			if (s == "let") return Token(let);
			if (s == "quit") return Token(quit);
			
			return Token(name, s);
		}
		error("Bad token");
	}
}


///Gets a char and checks if the token stream buffer is full
///
///While the input is not the char passed it will keep recieving

void Token_stream::ignore(char c)
{
	if (full && c == buffer.kind) {
		full = false;
		return;
	}
	full = false;

	char ch;
	while (cin >> ch)
		if (ch == c) return;
}

/// Variable with a constructor
struct Variable {
	string name;
	double value;
	bool isConstant;
	//Variable(string n, double v) :name(n), value(v), isConstant(f) { }
	Variable(string n, double v, bool t) :name(n), value(v), isConstant(t) { }
};

///Contains all variables
vector<Variable> names;



//////Recieves a string and tries to find a variable with that name
//////
//////If found returns its value
//////
//////Else prints an error
double get_value(string s)
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return names[i].value;
	
	error("get: undefined name ", s);
}

//////Recieves a string and a value. Tries to find a variable with that name
//////
//////If found sets the variable value to the one passed
//////
//////Else prints an error
void set_value(string s, double d)
{
	for (int i = 0; i <= names.size(); ++i)
		if (names[i].name == s) {
			names[i].value = d;
			return;
		}
	error("set: undefined name ", s);
}

//////Recieves a string and tries to find a variable with that name
//////
//////If found returns returns true to transmit that it already exist (True)
//////
//////Else returns false
bool is_declared(string s)
{
	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == s) return true;
	return false;
}

Token_stream ts;

double expression();
double declaration();



//////Gets a token kind
//////
//////If its kind is a parenthesis: call expression(), which assigns d (nothing is done with that - Consider checking)
//////
//////if its kind is a negative sign "-", recursion, but this time with a negative sign
//////
//////if its kind is a number: it returns the value of the token
//////
//////if its kind is a name: it returns the value of the token (Possible problem here)
//////
//////if none apply: Error
//////


double primary()
{
	Token t = ts.get();
	switch (t.kind) {
	case '(':
	{	double d = expression();
	t = ts.get();
	if (t.kind != ')') error("'(' expected");
	}
	case '-':
		return -primary();
	case number:
		return t.value;
	case name:
		return get_value(t.name);
	case let:
		return declaration();
	default:
		error("primary expected");
	}
}

//////Call the primary function and assign it to 'left'
//////
//////while True:
//////
//////			Check if a token kind is '*', '\'
//////
//////					if *: update left with primary() again and break
//////
//////					if \: check that there is no division by 0 and divide
//////
//////					if none apply: unget() and return original left
double term()
{
	double left = primary();
	while (true) {
		Token t = ts.get();
		switch (t.kind) {
		case number: {
		
			ts.unget(t);
			left *= primary();
		
		}
		case '*':
			left *= primary();
			break;
		case '/':
		{	double d = primary();
		if (d == 0) error("divide by zero");
		left /= d;
		break;
		}
		default:
			ts.unget(t);
			return left;
		}
	}
}

//////Call the term() function and assign it to 'left'
//////
//////while True:
//////
//////			Check if a token kind is '+', '-'
//////
//////					if +: add the result of term() with original left
//////
//////					if -: substract the result of term() with original left
//////
//////					if none apply: unget() and return original left
double expression()
{
	double left = term();
	while (true) {
		Token t = ts.get();

		switch (t.kind) {
		case '+':
			left += term();
			break;
		case '-':
			left -= term();
			break;
		default:
			ts.unget(t);
			return left;
		}
	}
}

///Allows the change of the value of a variable. Usefull if you are constantly modifying a value, but it may be possible to accidentally change a variable that wasn't suppossed to cahnge
void changeValue(string name, double d) {

	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == name) {

			names[i].value = d;
			return;
		}
}

bool isConst(string name) {

	for (int i = 0; i < names.size(); ++i)
		if (names[i].name == name) {

			return names[i].isConstant;
		}
}

//////Declares a Variable
//////
//////if Token kind is not 'a' - error (because it signals declaration)
//////
//////gets the name from the token
//////
//////Checks if there is a variable with the same name (If yes: error)
//////
//////Gets next token, expecting a equal sign to assign a value
//////
//////Use expression() to get the value		
//////		
//////Creates a Variable and add it to names
//////		
//////Return the variable value

double declaration()
{
	bool change = false;

	Token t = ts.get();
	if (t.kind != 'a') error("name expected in declaration");
	string name = t.name;

	if (is_declared(name)) {

		if (isConst(name))error("Attempted to change a constant value");

		else change = true;
	}

	Token t2 = ts.get();
	if (t2.kind != '=') error("= missing in declaration of ", name);

	double d = expression();

	if (change == false)
		names.push_back(Variable(name, d, false));

	else
		changeValue(name, d);

	return d;
}

//////Checks what statement is being passed
//////
//////if Token kind relates to 'let' it will try to create a variable
//////
//////else it will just return a expression
//////
double statement()
{

	Token t = ts.get();

	switch (t.kind) {
	case let: 
		return declaration();

	default: {

		ts.unget(t);

		return expression();
	}
		
	}
}
///Clean stream
void clean_up_mess()
{
	ts.ignore(print);
}

const string prompt = "> ";
const string result = "= ";


///Starter fucntion
void calculate()
{
	names.push_back(Variable("k", 1000, true));
	names.push_back(Variable("pi", 3.14, true));
	names.push_back(Variable("e", 2.72, true));

	//Run as long as there are no errors // Based around ts.get
	while (true) try {

		cout << prompt;
		Token t = ts.get();

		while (t.kind == print) t = ts.get();

		if (t.kind == quit) return;

		ts.unget(t);

		cout << result << statement() << endl;	
	}
	catch (runtime_error& e) {
		cerr << e.what() << endl;
		clean_up_mess();
	}
}

int main()

try {

	///Starter
	calculate();
	return 0;
}
catch (exception& e) {
	cerr << "exception: " << e.what() << endl;
	char c;
	while (cin >> c && c != ';');
	return 1;
}
catch (...) {
	cerr << "exception\n";
	char c;
	while (cin >> c && c != ';');
	return 2;
}
