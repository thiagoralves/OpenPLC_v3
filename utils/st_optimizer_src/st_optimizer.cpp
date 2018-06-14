//-----------------------------------------------------------------------------
// Copyright 2017 Thiago Alves
// This file is part of the OpenPLC Software Stack.
//
// OpenPLC is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// OpenPLC is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with OpenPLC.  If not, see <http://www.gnu.org/licenses/>.
//------
//
// This program is responsible for the optimization process after the initial
// compilation from PLCOpen Editor. All it does is to scan the ST file for
// first-level IF statements and concatenate identical statements together

// Thiago Alves, Sep 2017
//-----------------------------------------------------------------------------

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

string final_program;
vector<string> list_of_IFs;
int current_line = 0;

//-----------------------------------------------------------------------------
// Helper function - Verifies if the supplied line is an IF statement
//-----------------------------------------------------------------------------
bool is_IF_statement(const string &line)
{
	int i = 0;
	for (i = 0; i < line.length(); i++)
	{
		if (line.at(i) != ' ')
			break;
	}
	
	if (line.size() >= 3 && i < line.length())
	{
		if (line.at(i) == 'I' && line.at(i+1) == 'F' && line.at(i+2) == ' ')
		{
			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
// Helper function - Verifies if the supplied line is an END_IF statement
//-----------------------------------------------------------------------------
bool is_END_IF_statement(const string &line)
{
	int i = 0;
	for (i = 0; i < line.length(); i++)
	{
		if (line.at(i) != ' ')
			break;
	}
	
	if (line.size() >= 7)
	{
		if (line.at(i) == 'E' && line.at(i+1) == 'N' && line.at(i+2) == 'D' && line.at(i+3) == '_' && line.at(i+4) == 'I' && line.at(i+5) == 'F' && line.at(i+6) == ';')
		{
			return true;
		}
	}
	return false;
}

//-----------------------------------------------------------------------------
// Main function - All the magic happens here.
//-----------------------------------------------------------------------------
int main(int argc, char *argv[])
{
	//Verify if there are enough arguments
	if (argc < 3)
	{
		printf("You should provide the ST file to be optimized and the output file. \r\nEx: ./st_optimizer program.st output.st\r\n");
		return 0;
	}
	
	//Open file for reading
	ifstream stfile(argv[1]);
	if (stfile.is_open())
	{
		string line;
		//This big while loop scans the entire file looking for IF statements. All IF statements are
		//stored in the list_of_IFs vector
		while (getline(stfile, line))
		{
			current_line++;
			if (is_IF_statement(line))
			{
				//If this line is an IF statement, we first need to verify if it is a duplicate
				bool is_duplicate = false;
				for (vector<string>::iterator it = list_of_IFs.begin(); it != list_of_IFs.end(); it++)
				{
					if (!line.compare(*it))
					{
						is_duplicate = true;
					}
				}
				if (!is_duplicate)
				{
					//If the line is NOT a duplicate, we need to store it in the final program and
					//also store it in the list_of_IFs vector
					string if_statement(line);
					list_of_IFs.push_back(line);
					final_program.append(line + "\n");
					
					//This while loop will copy all contents of the IF statement until it finds the
					//END_IF terminator. Note: It will ignore sub-level IF statements
					int count_for_break = 1;
					while (getline(stfile, line))
					{
						current_line++;
						if(is_IF_statement(line))
							count_for_break++;
						else if(is_END_IF_statement(line))
							count_for_break--;
						if (count_for_break == 0)
							break;
						
						final_program.append(line + "\n");
					}
					
					string end_if(line);
					int current_position = current_line;
					
					//This while loop concatenates all identical IF statements together
					while (getline(stfile, line))
					{
						if (!line.compare(if_statement))
						{
							count_for_break = 1;
							while (getline(stfile, line))
							{
								if(is_IF_statement(line))
									count_for_break++;
								else if(is_END_IF_statement(line))
									count_for_break--;
								if (count_for_break == 0)
									break;
								
								final_program.append(line + "\n");
							}
						}
					}
					
					//For some reason ifstream.tellg() and ifstream.seekg() are not working,
					//so this is the work-around
					stfile.close();
					stfile.open(argv[1]);
					for (int i = 0; i < current_position; i++)
					{
						getline(stfile, line);
					}
					final_program.append(end_if + "\n");
				}
				else
				{
					//If this is a duplicated IF statement we just skip it with all its content
					int count_for_break = 1;
					while (getline(stfile, line))
					{
						current_line++;
						if(is_IF_statement(line))
							count_for_break++;
						else if(is_END_IF_statement(line))
							count_for_break--;
						if (count_for_break == 0)
							break;
					}
				}
			}
			else
			{
				final_program.append(line + "\n");
			}
		}
		
		stfile.close();
	}
	
	else
	{
		printf("Couldn't open file \"%s\"\r\n", argv[1]);
		return -1;
	}
	
	//Finally, this opens/create the file to write the optimized program
	ofstream outfile(argv[2], ios::trunc);
	if (outfile.is_open())
	{
		outfile << final_program;
		outfile.close();
	}
	else
	{
		printf("Couldn't write to output file \"%s\"\r\n", argv[2]);
		return -1;
	}
}
