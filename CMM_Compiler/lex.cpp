#include "lex.h"
#include <fstream>
#include <iostream>
#include <cctype>


using namespace std;

/*
	lex.cpp
	Tim Schneider
	CSC 306
*/


Token_Type Lex::get_token()
{
	static char ch = ' ';
	lexeme = "";

	while (isspace(ch))
	{
		ch = get_char();
	}
	// Now at a non-space

	// check for _ or alpha
	if (ch == '_' || isalpha(ch))
	{
		lexeme += ch;
		ch = get_char();

		// Get whole lexeme
		while (ch == '_' || isalpha(ch))
		{
			lexeme += ch;
			ch = get_char();
		}

		// Look in map for lexeme
		if (keywords_map.find(lexeme) != keywords_map.end())
		{
			return keywords_map.find(lexeme)->second;
		}
		else
		// Not in lexeme, must be id
		{
			return Token_Type::t_id;
		}
	}



	// Check if digit
	if (isdigit(ch))
	{
		lexeme += ch;
		ch = get_char();
		while (isdigit(ch))
		{
			lexeme += ch;
			ch = get_char();
		}
		return Token_Type::t_number;
	}

	// Every other token is a unique character

	switch (ch)
	{
	case '+':  lexeme += ch;
		ch = get_char();
		return Token_Type::t_plus;

	case '-': lexeme += ch;
		ch = get_char();
		return Token_Type::t_minus;

	case '*': lexeme += ch;
		ch = get_char();

		return Token_Type::t_mult;

	case '/': lexeme += ch;
		ch = get_char();
		if (ch == '/')
		{
			while (ch != '\n')
			{
				if (ch == -1)
				{
					fatal_error(" Program ended while scanning comment.");
					return Token_Type::t_eof;
				}
				ch = get_char();
			}
			ch = get_char();
			return get_token();
		}

		if (ch == '*')
		{
			ch = get_char();
			while (ch != '*')
			{
				ch = get_char();
				if (ch == -1)
				{
					fatal_error(" Program ended while scanning multi-line comment.");
					break;
				}
			}
			ch = get_char();
			if (ch == '/')
			{
				ch = get_char();
				return get_token();
			}
		}
		return Token_Type::t_div;

	case '%': lexeme += ch;
		ch = get_char();
		return Token_Type::t_mod;

	case '!': lexeme += ch;
		ch = get_char();
		if (ch == '=')
		{
			lexeme += ch;
			ch = get_char();
			return Token_Type::t_ne;
		}
		else
		{
			return Token_Type::t_not;
		}

	case '<': lexeme += ch;
		ch = get_char();
		if (ch == '=')
		{
			lexeme += ch;
			ch = get_char();
			return Token_Type::t_le;
		}

		if (ch == '<')
		{
			lexeme += ch;
			ch = get_char();
			return Token_Type::t_insertion;
		}

		return Token_Type::t_lt;


	case '>': lexeme += ch;
		ch = get_char();

		if (ch == '=')
		{
			lexeme += ch;
			ch = get_char();
			return Token_Type::t_ge;
		}

		if (ch == '>')
		{
			lexeme += ch;
			ch = get_char();
			return Token_Type::t_extraction;
		}

		return Token_Type::t_gt;


	case '=':
		lexeme += ch;
		ch = get_char();
		if (ch == '=')
		{
			lexeme += ch;
			ch = get_char();
			return Token_Type::t_eq;
		}
		else
		{
			lexeme += ch;
			return Token_Type::t_assign;
		}

	case ',': lexeme += ch;
		ch = get_char();
		return Token_Type::t_comma;

	case ';': lexeme += ch;
		ch = get_char();
		return Token_Type::t_semi;

	case '(': lexeme += ch;
		ch = get_char();
		return Token_Type::t_lparen;

	case ')': lexeme += ch;
		ch = get_char();
		return Token_Type::t_rparen;

	case '"':
		ch = get_char();
		while (ch != '"')
		{
			if (ch == -1)
			{
				fatal_error("Program ended during a String");
				return Token_Type::t_eof;
			}
			lexeme += ch;
			ch = get_char();
		}

		ch = get_char();
		return Token_Type::t_string;


	default:
		lexeme += ch;
		ch = get_char();

		if (ch == -1)
		{
			return Token_Type::t_eof;
		}
		return Token_Type::t_unknown;
	}

}


// Lexical Analyzer
Lex::Lex(istream* in, ostream* out)
{
	source_stream = in;
	listing_stream = out;
}

// Lex destructor
Lex::~Lex()
{

}


// Note: order of the vector entries is tied to order of Token_Type constants!
static const vector < string > token_tostring
{
	"t_begin", "t_break", "t_cin", "t_continue", "t_cout",
	"t_do", "t_else", "t_end", "t_if",
	"t_int", "t_loop", "t_then", "t_while", 
	"t_id", "t_number", "t_string", "t_plus", "t_minus", "t_mult",
	"t_div", "t_mod", "t_assign", "t_not",
	"t_lt", "t_le", "t_gt", "t_ge", "t_ne", "t_eq",
	"t_insertion", "t_extraction",
	"t_comma", "t_semi", "t_lparen", "t_rparen",
	"t_unknown", "t_eof", 
	"exit", "newline",

};

static const map<string, Token_Type> keywords_map
{
	{ "begin", Token_Type::t_begin },
	{ "break", Token_Type::t_break },
	{ "cin", Token_Type::t_cin },
	{ "continue", Token_Type::t_continue },
	{ "cout", Token_Type::t_cout },
	{ "do", Token_Type::t_do },
	{ "else", Token_Type::t_else },
	{ "end", Token_Type::t_end },
	{ "if", Token_Type::t_if },
	{ "int", Token_Type::t_int },
	{ "loop", Token_Type::t_loop },
	{ "then", Token_Type::t_then },
	{ "while", Token_Type::t_while },
	{ "exit", Token_Type::t_exit },
	{ "newline", Token_Type::t_newline },
};


