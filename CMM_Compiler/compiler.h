#include <cstdlib>
#include <cassert>
#include "lex.h"
#include <set>
#include "CmmVirtualMachine.h"

using namespace std;

/*  
	compiler.h
	Tim Schneider
	CSC 306
*/

extern const set<Token_Type> relops;
extern const set<Token_Type> addops;
extern const set<Token_Type> multops;
extern const set<Token_Type> expr_begin;
extern const set<Token_Type> stmt_begin;


class Compiler
{
public:
	Compiler(istream* source_stream, ostream* listing_stream)
		:
		lex{ source_stream, listing_stream }
	{
		// get the lookahead token

		token = lex.get_token();
	}

	void compile();

private:

	// Parsing methods for non-terminals
	void vars(); void vardeclist();
	void type_id(); void stmtlist(); void stmt();
	void assignstmt(); void outputstmt(); void inputstmt();
	void ifstmt(); void whilestmt(); void loopstmt();

	void output_expr(cmm_super_expr*&);
	void expr(cmm_expr*&);
	void comp_expr(cmm_expr*&);
	void simple_expr(cmm_expr*&);
	void factor(cmm_expr*&);

	// Various utilities for token classification
	bool is_relop(Token_Type t)
	{
		return relops.find(t) != relops.end();
	}
	bool is_multop(Token_Type t)
	{
		return multops.find(t) != multops.end();
	}
	bool is_addop(Token_Type t)
	{
		return addops.find(t) != addops.end();
	}
	bool is_stmt_begin(Token_Type t)
	{
		return stmt_begin.find(t) != stmt_begin.end();
	}
	bool is_expr_begin(Token_Type t)
	{
		return expr_begin.find(t) != expr_begin.end();
	}

	// Recognize an expected token
	void accept(Token_Type t)
	{
		if (token == t)
		{
			token = get_token();
		}
		else
		{
			syntax_error(token, { t });
		}
	}

	// Member variables

	// lexical analyzer
	Lex lex; 

	// lookahead token
	Token_Type token;

	// Interface to lexical analyzer
	inline Token_Type get_token() { return lex.get_token(); }
	inline string get_lexeme() { return lex.get_lexeme(); }

	// Fatal error method
	void Compiler_fatal_error(const string &message)
	{
		lex.fatal_error(message);
		exit(1);
	}

	// Non-fatal error method
	void error(const string &message)
	{
		non_fatal_error_count++;
		lex:error(message);
	}

	// Error message for when an unexpected token is found.
	void syntax_error(Token_Type found, initializer_list<Token_Type> expected);

	// Generic error message
	void syntax_error(const string &message)
	{
		Compiler_fatal_error(message);
	}
};
