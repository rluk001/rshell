#include <cstring>
#include <iostream>
#include <string>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <pwd.h>
#include <vector>
using namespace std;

void printUserInfo()
{
	char *login = getpwuid(getuid())->pw_name;
	char hostname[1024];
	int gethost = gethostname(hostname, sizeof(hostname)-1);
	if(login != NULL && gethost == 0)
	{
		cout << login << "@" << hostname << "$ ";
	}
	else
	{
		cout << "$ ";
	}
}

int execute(char ** argv)
{	
	int stat = 0;
	int pid = fork();
	if(pid == -1)
	{
		perror("Error: fork failed");
		exit(1);
	}
	else if(pid > 0) 	// Parent process
	{
		if(wait(&stat) == -1)
		{
			perror("Error: wait has failed");
			exit(1);
		}
	}
	else if(pid == 0)	// Child process (exec)
	{
		if(execvp(argv[0], argv) == -1)
		{
			perror("Error: execvp failed");
			exit(1);
		}
	}
	return stat;
}

void commentCheck(string & input)
{
	bool commCheck = false;
	int commIndex = 0;
	for(int i = 0; i < input.size(); i++)
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

char ** parse(char * line, const char *delim)
{
	vector <char *> cArray;
	char * del = strtok(line, delim);
	while(del != NULL)
	{
		cArray.push_back(del);
		del = strtok(NULL, delim);
	}
	char **argvCount = new char *[cArray.size()+1];
	argvCount[cArray.size()] = 0;
	
	for(int i = 0; i < cArray.size(); i++)
	{
		argvCount[i] = new char[cArray.size() + 1];
		strcpy(argvCount[i], cArray[i]);
	}
	return argvCount;
}

int parseArgs(char * line)
{
	char ** parsedSpaces = parse(line, " ");
	int executeProgram = execute(parsedSpaces);
	for(int i = 0; parsedSpaces[i] != NULL; i++)
	{
		delete parsedSpaces[i];
	}
	delete parsedSpaces;
	return executeProgram;
}

void parseLogicOps(char * line)
{
	char * orFind = strchr(line, '|');
	char * andFind = strchr(line, '&');
	char ** parsedTokens = parse(line, "&|");
	bool orTF = (orFind != NULL);
	bool andTF = (andFind != NULL);
	for(int i = 0; parsedTokens[i] != NULL; i++)
	{
		if(orTF)
		{
			if((parseArgs(parsedTokens[i])) == 0)
			{
				return;
			}
		}
		else if(!orTF)
		{
			if((parseArgs(parsedTokens[i])) != 0)
			{
				return;
			}
		}
		/*else if(andTF)
		{
			if(parseForArgs(parsedTokens[i]) == 0)
			{
				return;
			}
		}*/
	}
	for(int i = 0; parsedTokens[i] != NULL; i++)
	{
		delete parsedTokens[i];
	}
	delete parsedTokens;	
}

void parseCommands(char * line)
{
	char ** parsedSemis = parse(line, ";");
	for(int i = 0; parsedSemis[i] != NULL; i++)
	{
		parseLogicOps(parsedSemis[i]);
	}
	for(int i = 0; parsedSemis[i] != NULL; i++)
	{
		delete parsedSemis[i];
	}
	delete parsedSemis;
}

int main()
{
	cout << "Initializing Shell... " << endl << endl;
	string input;
	char * line;
	do
	{
		printUserInfo();	
		getline(cin, input);
		commentCheck(input);
		if(input == "exit")
		{
			exit(1);
		}
		line = new char[input.size()+1];
		strcpy(line, input.c_str());
		parseCommands(line);
		delete line;
	}while(input != "exit");	
	return 0;
}
