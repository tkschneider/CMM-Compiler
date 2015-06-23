#include "CmmVirtualMachine.h"
#include <map>
#include <stack>

// Global table variables
vector<string> string_table;
vector<cmm_stmt *> instruction_table;
cmm_variable_table  variable_table;

// positions of top of loop in the instruction table
stack<int> loop_top_stack;

// stack of lists of break statements that appear in a loop.
stack<vector<break_stmt *>> break_stmts_stack;

// Non-fatal error counter and program counter 
int non_fatal_error_count = 0;
int cmm_pc = 0;

// Execute method that executes statements in instruction table
void cmm_execute()
{
	if (non_fatal_error_count > 0)
	{
		throw "CMM program has errors.\n";
	}

	while (true)
	{
		cmm_stmt * p_current_stmt = instruction_table[cmm_pc];
		//cerr << "executing statement at " << cmm_pc << endl;
		cmm_pc++;
		p_current_stmt->execute();
	}
}

// Methods for printing tables
void string_table_print(ostream &out)
{
	out << "String Table" << endl;
	for (unsigned int k = 0; k < string_table.size(); k++)
	{
		out << k << " : " << string_table[k] << endl;
	}
}
void variable_table_print(ostream &out)
{
	out << "Variable Table" << endl;
	for (unsigned int k = 0; k < variable_table.size(); k++)
	{
		out << k << ": " << variable_table[k]->get_id() << endl;
	}
}
void instruction_table_print(ostream &out)
{
	out << "Instruction Table" << endl;
	for (unsigned int k = 0; k < instruction_table.size(); k++)
	{
		out << k << ": " << instruction_table[k]->to_string() << endl;
	}
}



/**
* Searches a variable table for an entry for a given id
* and returns the index of the id within the table, or -1.
* @param id
* @return
*/
int cmm_variable_table::index_of(string id)
{
	int index = size() - 1;
	for (; index >= 0; index--)
	{
		if ((*this)[index]->get_id() == id)
			return index;
	}
	return index;
}

// to_string member function
string unary_op_expr::to_string()
{
	ostringstream s{};
	switch (op)
	{
	case unary_op::minus_op:
		s << "-"; break;
	case unary_op::not_op:
		s << "!"; break;
	}
	s << this->p_expr->to_string();
	return s.str();
}

// get_value member function
int unary_op_expr::get_value()
{
	switch (op)
	{
	case unary_op::minus_op:
		return -this->p_expr->get_value();
	case unary_op::not_op:
		return !this->p_expr->get_value();
	}
}

// to_string member function
map<binary_op, string> binary_op_to_string_map
{
	{ binary_op::div_op, "/" },
	{ binary_op::mult_op, "*" },
	{ binary_op::mod_op, "%" },
	{ binary_op::plus_op, "+" },
	{ binary_op::minus_op, "-" },
	{ binary_op::lt_op, "<" },
	{ binary_op::le_op, "<=" },
	{ binary_op::gt_op, ">" },
	{ binary_op::ge_op, ">=" },
	{ binary_op::ne_op, "!=" },
	{ binary_op::eq_op, "==" },
};

// to_string member function
string binary_op_expr::to_string()
{
	ostringstream s{};
	s << "(" << this->p_expr1->to_string() << ")"
		<< binary_op_to_string_map[this->op]
		<< "(" << this->p_expr2->to_string() << ")";
	return s.str();
}

// to_string member function
string output_stmt::to_string()
{
	ostringstream s{ "output: " };
	s << "cout  ";
	for (auto p : this->output_exprs)
	{
		s << "[" << p->to_string() << "] ";
	}
	return s.str();
}

// to_string member function
string input_stmt::to_string()
{
	ostringstream s{};
	s << "cin ";
	for (auto id : this->var_ids)
	{
		s << "[" << id << "] ";
	}
	return s.str();
}




