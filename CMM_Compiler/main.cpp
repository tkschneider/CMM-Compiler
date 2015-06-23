#include  "compiler.h"
#include <fstream>
#include <iostream>

using namespace std;

/*
	main.cpp
	CMM Compiler
	Author: Tim Schneider
*/

int main(int argc, char * argv[])
{
	// Get name of source and listing files from the command line
	if (argc != 3)
	{
		cout << "Specify name of source and listing files on the command line:"
			<< "\n" << argv[0] << " sourceFile  listingFile" << endl;
		return 1;
	}
	// Open the source and listing files:
	ifstream sourcefile(argv[1]);
	ofstream listingfile(argv[2]);

	if (!sourcefile)
	{
		cout << "Cannot open the file " << argv[1] << endl;
	}
	if (!listingfile)
	{
		cout << "Cannot open the file " << argv[2] << endl;
	}
	if (!sourcefile || !listingfile) { return 2; }

	// source and listing files are good, so create compiler object
	Compiler compiler(&sourcefile, &listingfile);

	// compile
	compiler.compile();

	// dump the string table, variable table and instruction table
	// to the listing file
	listingfile << endl << endl;
	string_table_print(listingfile);
	listingfile << endl << endl;
	variable_table_print(listingfile);
	listingfile << endl << endl;
	instruction_table_print(listingfile);

    if (::non_fatal_error_count > 0)
    {
        listingfile << "\n\nThere are errors in the cmm program." << endl;
        exit(3);
    }

	// Execute Statements
	cmm_execute();

	return 0;
}
