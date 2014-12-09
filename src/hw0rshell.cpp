#include <cstring>
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <cstdlib>
#include <string.h>
#include <pwd.h>
#include <vector>
#include <signal.h>
using namespace std;

bool exitCall = false;

void printUserInfo() // Prints pwname and hostname
{
	char *login = getpwuid(getuid())->pw_name;
	if(login == NULL)
	{
		perror("Error: getpwuid failed");
	}
	char hostname[1024];
	int gethost = gethostname(hostname, sizeof(hostname)-1);
	if(gethost == -1)
	{
		perror("Error:gethostname failed");
	}
	if(login != NULL && gethost == 0)
	{
		cout << login << "@" << hostname << "$ ";
	}
	else
	{
		cout << "$ ";
	}
}

int execute(char ** argv) // Execute for regular commands
{	
	int stat = 0;
	int pid = fork();
	if(pid == -1)
	{
		perror("Error: fork failed");
		exit(1);
	}
	else if(pid == 0)	// Child process (exec)
	{
		if(execvp(argv[0], argv) == -1)
		{
			perror("Error: execvp failed");
		}
		exitCall = true;
		return -1;
	}
	else if(pid > 0) 	// Parent process
	{
		if(wait(&stat) == -1)
		{
			perror("Error: wait has failed");
			exit(1);
		}
	}
	return stat;
}

void commentCheck(string & input) // If there is a comment if gets erased
{
	bool commCheck = false;
	int commIndex = 0;
	for(unsigned int i = 0; i < input.size(); i++)
	{
		if(input[i] == '#')
		{
			commIndex = i;
			commCheck = true;
			break;
		}
	}
	if(commCheck)
	{
		input =	input.substr(0, commIndex);
	}
}

char ** parse(char * line, const char *delim) // parse by delimiter
{
	char ** argvCount = new char * [1024];
	memset(argvCount, 0, 1024);
	
	char * tokens = strtok(line, delim);
	int i = 0;
	while(tokens != NULL)
	{
		argvCount[i] = tokens;
		i++;
		tokens = strtok(NULL, delim);
	}
	argvCount[i] = NULL;

	return argvCount;
}

int parseArgs(char * line) // parseSpaces
{
	char ** parsedSpaces = parse(line, " ");
	int executeProgram = execute(parsedSpaces);
	delete [] parsedSpaces;
	return executeProgram;
}

string removeSpaces(string line)
{
	string temp;
	for(unsigned int j = 0; j < line.size(); j++)
	{
		if(line[j] != ' ')
		{
			temp += line[j];
		}	
	}
	return temp;
}

void parseLogicOps(char * line)
{
	char * andFind = strchr(line, '&');
	bool andTF = (andFind != NULL);

	if(andTF && line[andFind-line+1] == '&')
	{
		char ** parsedAnd = parse(line, "&");
		for(unsigned int i = 0; parsedAnd[i]; i++)
		{
			char * orFind = strchr(parsedAnd[i], '|');
			bool orTF = (orFind != NULL);
			if(orTF && parsedAnd[i][orFind-parsedAnd[i]+1] == '|')
			{
				char ** parsedOr = parse(parsedAnd[i], "|");
				for(unsigned int j = 0; parsedOr[j]; j++)
				{
					string findExit = "";
					findExit += parsedOr[j];
					findExit = removeSpaces(findExit);
					if(findExit == "exit")
					{
						//exit(1);
						exitCall = true;
						break;
					}
					if(parseArgs(parsedOr[j]) == 0)
					{
						break;
					}
				}
				delete [] parsedOr;						
			}
			else
			{
				string findExit = "";
				findExit += parsedAnd[i];
				findExit = removeSpaces(findExit);
				if(findExit == "exit")
				{
					//exit(1);
					exitCall = true;
					break;
				}
				if(parseArgs(parsedAnd[i]) != 0)
				{
					/*delete [] parsedAnd;
					return -1;*/
					return ;
				}
			}
		}
		delete [] parsedAnd;
	}
	else 
	{
		char * orFindThis = strchr(line, '|');
		bool orTrueFalse = (orFindThis != NULL);
		if(orTrueFalse && line[orFindThis-line+1] == '|')
		{
			char ** parsedORS = parse(line, "|");
			for(unsigned int i = 0; parsedORS[i]; i++)
			{
				string findExit = "";
				findExit += parsedORS[i];
				findExit = removeSpaces(findExit);
				if(findExit == "exit")
				{
					//exit(1);
					exitCall = true;
					break;
				}
				if(parseArgs(parsedORS[i]) == 0)
				{
					break;
				}
			}
			delete [] parsedORS;
		}
		else
		{
			string findExit = "";
			findExit += line;
			findExit = removeSpaces(findExit);
			if(findExit == "exit")
			{
				//exit(1);
				exitCall = true;
			}
			if(parseArgs(line) != 0)
			{
				return ;
			}
		}
	}
}

void parseCommands(char * line)
{
	char ** parsedSemis = parse(line, ";");
	for(unsigned int i = 0; parsedSemis[i]; i++)
	{
		parseLogicOps(parsedSemis[i]);
	}
	delete [] parsedSemis;
}

string separateWithSpaces(string line) // Separates to make the i/o redirection in its own argv[i]
{
	string newString;
	for(unsigned int i = 0; i < line.size(); i++)
	{
		if(i != 0 && (line[i] == '|' && (line[i-1] != ' ' && line[i-1] != '|') && (line[i+1] != ' ' && line[i+1] != '|')))
		{
			newString += " | ";
		}
		else
		{
			newString += line[i];
		}
	}
	return newString;
}

int main()
{
	while(1)
	{
		if(exitCall)
		{
			break;
		}
		string input = "";
		printUserInfo();	
		getline(cin, input);
		commentCheck(input);	
		while(input == "")
		{
			printUserInfo();
			getline(cin, input);
		}
		input = separateWithSpaces(input);
		if(input == "exit")
		{
			break;
		}
		char * line = new char[input.size()+1];
		memset(line, 0, input.size()+1);
		strcpy(line, input.c_str());
		parseCommands(line);
		delete [] line;
	}
	return 0;
}
