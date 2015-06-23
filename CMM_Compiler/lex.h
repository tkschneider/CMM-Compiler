#include <string>
#include <vector>
#include <fstream>
#include <iostream>
#include <map>

using namespace std;

/*
	lex.h
	Tim Schneider
	CSC 306
*/

enum class Token_Type
{
	// keywords
	t_begin, t_break, t_cin, t_continue, t_cout, t_do, t_else, t_end, t_if,
	t_int, t_loop, t_then, t_while,
	// identifier, number, and string tokens
	t_id, t_number, t_string,
	// various operators
	t_plus, t_minus, t_mult, t_div, t_mod, t_assign, t_not,
	// relational operators

	t_lt, t_le, t_gt, t_ge, t_ne, t_eq,
	// io operators
	t_insertion, t_extraction,
	// various punctuation symbols
	t_comma, t_semi, t_lparen, t_rparen,
	// unknown and eof tokens
	t_unknown, t_eof,
	// exit and newline
	t_exit, t_newline
};

// These establish the correspondence between tokens and
// the stringfyed versions of those tokens, for example
// the t_begin token corresponds to the string "t_begin"
extern const vector < string > token_tostring;
extern const map<string, Token_Type> keywords_map;
extern int non_fatal_error_count;

class Lex
{

public:

	Token_Type get_token();
	string get_lexeme()
	{
		return lexeme;
	}
	string token_stringfy(Token_Type t)
	{
		return token_tostring[static_cast<int> (t)];
	}
	void error(const string &message)
	{
		(*listing_stream) << "*** Error *** " << message << endl;
		        non_fatal_error_count++;
	}
	void fatal_error(const string& message)
	{
		(*listing_stream) << "*** Error *** " << message << endl;
		exit(1);
	}
	// Constructor
	Lex(istream* source_filename, ostream* listing_filename);
	// Destructor
	~Lex();


private:
	// Get next character from file and echo to listing file
	char get_char()
	{
		char ch = source_stream->get();
		if (ch != -1) {
			listing_stream->put(ch);
		}
		return ch;
	}
	// various member variables
	string lexeme;
	istream* source_stream;
	ostream* listing_stream;
};