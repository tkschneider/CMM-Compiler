#include "Compiler.h"
#include "CmmVirtualMachine.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <cctype>

using namespace std;

/* 
	Compiler.cpp
	Tim Schneider
	CSC 306
*/

// Compile Method
void Compiler::compile()
{
	if (token == Token_Type::t_int || token == Token_Type::t_begin)
	{
		vars();
		accept(Token_Type::t_begin);
		stmtlist();
		instruction_table.push_back(new exit_stmt());
	}
	else
    {
        syntax_error(token,{Token_Type::t_int, Token_Type::t_begin});
    }
}

void Compiler::vars()
{
	while (token == Token_Type::t_int)
	{
		token = get_token();
		vardeclist();
	}
}

void Compiler::vardeclist()
{
	string id = get_lexeme();
	accept(Token_Type::t_id);
	// Add var to table if not in table
	if (variable_table.index_of(id) == -1)
	{
		variable_table.add(id);
	}
	else
	{
		syntax_error("Variable" + id + " already declared");
	}

	// Check for more declarations of variables and add them
	while (token == Token_Type::t_comma)
	{
		token = get_token();
		id = get_lexeme();
		if (variable_table.index_of(id) == -1)
		{
			variable_table.add(id);
		}
		else
		{
			syntax_error("Variable " + id + " already declared");
		}

		accept(Token_Type::t_id);
	}

	accept(Token_Type::t_semi);
}

void Compiler::type_id()
{
	accept(Token_Type::t_int);
}

void Compiler::stmtlist()
{
	// accept statements 
	while (is_stmt_begin(token))
	{
		stmt();
		accept(Token_Type::t_semi);
	}
}

void Compiler::stmt()
{
	break_stmt* p;

	switch (token)
	{
	case Token_Type::t_id:
		assignstmt();
		break;

	case Token_Type::t_cout:
		outputstmt();
		break;

	case Token_Type::t_cin:
		inputstmt();
		break;

	case Token_Type::t_if:
		ifstmt();
		break;

	case Token_Type::t_while:
		whilestmt();
		break;

	case Token_Type::t_loop:
		loopstmt();
		break;



	case Token_Type::t_continue:
		token = get_token();
        if(loop_top_stack.size()==0)
        {
            lex.error("Continue statement encountered outside of loop");
            return;
        }
        instruction_table.push_back(new continue_stmt(loop_top_stack.top()));
		break;

	case Token_Type::t_exit:
		instruction_table.push_back(new exit_stmt());
		token = get_token();
		break;

	case Token_Type::t_newline: 
		instruction_table.push_back(new newline_stmt());
		token = get_token();
		break;

	case Token_Type::t_break:
		token = get_token();
        //instruction_table.push_back(new break_stmt(-1));
        if(break_stmts_stack.size()==0)
        {
            lex.error("Break statement encountered outside of loop");
            return;
        }
		p = new break_stmt(-1);
        instruction_table.push_back(p);
        break_stmts_stack.top().push_back(p);
		break;

	default: syntax_error("Statement not found");
	}
}

void Compiler::assignstmt()
{
	string id = get_lexeme();
	assign_stmt* assign;
	cmm_expr* pExpr;
	int index;

	token = get_token();
	accept(Token_Type::t_assign);
	index = variable_table.index_of(id);
	// Check variable table for id
	if (index == -1)
	{

		// Add to table
		syntax_error("Indentifier not declared");
		variable_table.add(id);
		index = variable_table.index_of(id);
	}

	//Get expression and push assign statement to instruct table
	expr(pExpr);
	assign = new assign_stmt(id, pExpr);
	instruction_table.push_back(assign);
}

void Compiler::outputstmt()
{

	output_stmt* out = new output_stmt();

	// Accept cout and insertion
	token = get_token();
	accept(Token_Type::t_insertion);

	// Get expression and add to out
	cmm_super_expr* sExpr;
	output_expr(sExpr);
	out->add_expr(sExpr);

	// Check for more statements
	while (token == Token_Type::t_insertion)
	{
		token = get_token();
		output_expr(sExpr);
		out->add_expr(sExpr);
	}

	// Push statement list to table
	instruction_table.push_back(out);
}

void Compiler::inputstmt()
{
    accept(Token_Type::t_cin);
    input_stmt* inp = new input_stmt();
    do
    {
        accept(Token_Type::t_extraction);
        string id = lex.get_lexeme();
        accept(Token_Type::t_id);
        inp->add_var_id(id);
    }
    while (token == Token_Type::t_extraction);
    instruction_table.push_back(inp);
}

void Compiler::ifstmt()
{
	  
    accept(Token_Type::t_if);
    cmm_expr* condp;
    expr(condp);
    if_false_goto_stmt *if_false = new top_if_stmt(condp);
    instruction_table.push_back(if_false);
    accept(Token_Type::t_then);
    stmtlist();
    goto_stmt *elsegotoend;
    elsegotoend = new goto_stmt(-1);
    if (token == Token_Type::t_else)
    {
        token = get_token();
        
        instruction_table.push_back(elsegotoend);
        marker_stmt *marker = new marker_stmt("ELSE");

        if_false->patch(instruction_table.size());
        instruction_table.push_back(marker);
        stmtlist();
        elsegotoend->patch(instruction_table.size());
    }
    else
    {
        if_false->patch(instruction_table.size());
    }
    accept(Token_Type::t_end);
    accept(Token_Type::t_if);
    marker_stmt *endif = new marker_stmt("end if");
    
    instruction_table.push_back(endif);
    
}

void Compiler::whilestmt()
{
    accept(Token_Type::t_while);
    cmm_expr* condp;
    
    expr(condp);
    top_while_stmt *topwhile = new top_while_stmt(condp);
    int ind = instruction_table.size();
    loop_top_stack.push(ind);
    break_stmts_stack.push(vector<break_stmt*>());
    instruction_table.push_back(topwhile);
    accept(Token_Type::t_do);
    
    stmtlist();
    
    accept(Token_Type::t_end);
    accept(Token_Type::t_while);
    
    goto_stmt *gototop = new goto_stmt(ind);
    instruction_table.push_back(gototop);
    
    
    marker_stmt *marker = new marker_stmt("End of while");
    topwhile->patch(instruction_table.size());
    instruction_table.push_back(marker);
    loop_top_stack.pop();
    for(auto it = break_stmts_stack.top().begin();it!=break_stmts_stack.top().end();it++)
    {
        (*it)->patch(instruction_table.size());
    }
    break_stmts_stack.pop();
}

void Compiler::loopstmt()
{
    accept(Token_Type::t_loop);
    marker_stmt *toploop = new marker_stmt("top of loop");
    int ind = instruction_table.size();
    loop_top_stack.push(ind);
    break_stmts_stack.push(vector<break_stmt*>());
    instruction_table.push_back(toploop);
    stmtlist();
    accept(Token_Type::t_end);
    accept(Token_Type::t_loop);
    goto_stmt *gototop = new goto_stmt(ind);
    instruction_table.push_back(gototop);
    loop_top_stack.pop();
    for(auto it = break_stmts_stack.top().begin();it!=break_stmts_stack.top().end();it++)
    {
        (*it)->patch(instruction_table.size());
    }
    break_stmts_stack.pop();
}


void Compiler::output_expr(cmm_super_expr*& pExpr)
{
	string str;
	cmm_expr* outexpr;
	cmm_string_expr* strExpr;
	int index;

	// Check if string or expression
	if (token == Token_Type::t_string)
	{
		// Add string to string table
		index = string_table.size();
		str = get_lexeme();
		string_table.push_back(str);
		token = get_token();
		strExpr = new cmm_string_expr(index);
		// Return string exression
		pExpr = strExpr;
	}
	else
	{
		// Not string so expression 
		expr(outexpr);
		pExpr = outexpr;
	}
}

void Compiler::expr(cmm_expr*& pExpr)
{
	cmm_expr* expr1;
	binary_op bop;

	// Get comp+_expr
	comp_expr(pExpr);

	// Check relop
	if (is_relop(token))
	{
		// Identify relop type
		switch (token)
		{
		case Token_Type::t_lt: bop = binary_op::lt_op; break;
		case Token_Type::t_le: bop = binary_op::le_op; break;
		case Token_Type::t_gt: bop = binary_op::gt_op; break;
		case Token_Type::t_ge: bop = binary_op::ge_op; break;
		case Token_Type::t_ne: bop = binary_op::ne_op; break;
		case Token_Type::t_eq: bop = binary_op::eq_op; break;
		}
		token = get_token();
		//Get second comp_expr
		comp_expr(expr1);
		// Create new binary operator
		pExpr = new binary_op_expr(bop, pExpr, expr1);
	}
}

void Compiler::comp_expr(cmm_expr*& pExpr)
{
	binary_op bop;
	cmm_expr* expr1;
	// Get simple_expr
	simple_expr(pExpr);
	// Check for addition operator
	while (is_addop(token))
	{
		// Identify operator
		switch (token)
		{
		case Token_Type::t_plus: bop = binary_op::plus_op; break;
		case Token_Type::t_minus: bop = binary_op::minus_op; break;
		}
		token = get_token();
		// Get simple_expr
		simple_expr(expr1);
		pExpr = new binary_op_expr(bop, pExpr, expr1);
	}
}

void Compiler::simple_expr(cmm_expr*& pExpr)
{
	binary_op bop;
	cmm_expr* expr1;

	// Get factor
	factor(pExpr);
	// Check for multop
	while (is_multop(token))
	{
		// Identify multop
		switch (token)
		{
		case Token_Type::t_div: bop = binary_op::div_op; break;
		case Token_Type::t_mult: bop = binary_op::mult_op; break;
		case Token_Type::t_mod: bop = binary_op::mod_op; break;
		}
		token = get_token();
		// Get factor
		factor(expr1);
		// Create and return new expression
		pExpr = new binary_op_expr(bop, pExpr, expr1);;
	}
}

void Compiler::factor(cmm_expr*& pExpr)
{
	int index;
	switch (token)
	{
	case Token_Type::t_number:
		pExpr = new number_expr(std::stoi(get_lexeme()));
		token = get_token();
		break;

	case Token_Type::t_id:
		index = variable_table.index_of(get_lexeme());
		if (index == -1)
		{
			syntax_error("Indentifier not declared");
			variable_table.add(get_lexeme());
			index = variable_table.index_of(get_lexeme());
		}
		pExpr = new id_expr(index);
		token = get_token();
		break;

	case Token_Type::t_not:
		token = get_token();
		cmm_expr* not_factor;
		factor(not_factor);
		pExpr = new unary_op_expr(unary_op::not_op, not_factor);
		break;

	case Token_Type::t_minus:
		token = get_token();
		cmm_expr* min_factor;
		factor(min_factor);
		pExpr = new unary_op_expr(unary_op::minus_op, min_factor);
		break;

	case Token_Type::t_lparen:
		token = get_token();
		expr(pExpr);
		accept(Token_Type::t_rparen);
		break;

	default: syntax_error("Factor not found");
		cout << get_lexeme();
	}
}
//////////////////////////


void Compiler::syntax_error(Token_Type found, initializer_list<Token_Type> expected)
{
	assert(expected.size() != 0);
	ostringstream message{};
	message << "Expected ";
	if (expected.size() > 1)
	{
		message << " one of : ";
	}
	for (auto p = expected.begin();;)
	{
		message << lex.token_stringfy(*p);
		p++;
		//is it the last one?
		if (p != expected.end())
		{
			message << ", ";
			continue;
		}
		break;
	}
	message << " but found " << lex.token_stringfy(found);
	Compiler_fatal_error(message.str());
}

// member variables for token classification
extern const set<Token_Type> stmt_begin // start symbols for STMT
{
	Token_Type::t_id, Token_Type::t_cout, Token_Type::t_cin, Token_Type::t_if,
	Token_Type::t_while, Token_Type::t_loop, Token_Type::t_break, Token_Type::t_continue,
	Token_Type::t_newline, Token_Type::t_exit
};
extern const set<Token_Type> expr_begin // start symbols for expressions
{
	Token_Type::t_number, Token_Type::t_id, Token_Type::t_lparen,
	Token_Type::t_not, Token_Type::t_minus,
};
extern const set<Token_Type> relops
{
	Token_Type::t_lt, Token_Type::t_le, Token_Type::t_gt,
	Token_Type::t_ge, Token_Type::t_ne, Token_Type::t_eq,
};
extern const set<Token_Type> addops
{
	Token_Type::t_plus, Token_Type::t_minus,
};
extern const set<Token_Type> multops
{
	Token_Type::t_mult, Token_Type::t_div, Token_Type::t_mod,
};