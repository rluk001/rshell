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
void parse(char * line, char ** argv, int &argNum, char * delim)
{
	char *parsedWord;
	parsedWord = strtok(line, delim); // parses to first delimiter
	while(parsedWord != NULL)
	{
		argv[argNum] = parsedWord;
		argNum++;
		parsedWord = strtok(NULL, delim); // parses from then on
	}
	argv[argNum] = NULL; // makes the last NULL
}

void execute(char ** argv)
{
	int pid = fork();
	if(pid == -1) // If fork fails
	{
		perror("Error: fork failed");
		exit(1);
	}
	else if(pid > 0) 	// Parent process
	{
		if(wait(0) == -1)
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
}

void printUserInfo()
{
	char *login = getpwuid(getuid())->pw_name; // Get login with pwd.h
	char hostname[1024];
	int gethost = gethostname(hostname, sizeof(hostname)-1); // Get host
	if(login != NULL && gethost == 0)
	{
		cout << login << "@" << hostname << "$ ";
	}
	else
	{
		cout << "$ ";
	}
}

void commentCheck(char *  line)
{
	bool commCheck = false;
	int commIndex = 0;
	for(int i = 0; i < strlen(line); i++)
	{
		if(line[i] == '#') // Checks if there is a '#' sign
		{
			commIndex = i;
			commCheck = true;
		}
	}
	if(commCheck) // Runs if it finds a '#' key
	{
		for(int i = commIndex; i < strlen(line) - commIndex; i++)
		{
			line[i] = 0;
		}
	}
}
int main()
{
	while(1) //a never-ending loop unless presented with exit and perror cases
	{
		printUserInfo();	
		char line[1024] = {0};
		char ** argv;
		int argNum = 0;
		char delimSpace[] = " ";
		char delimORs[] = "||";
		char delimANDs[] = "&&";
		char delimSEMis[] = ";";
		//vector <char **> cArray;
		cin.getline(line, 1024);
		commentCheck(line);	// checks for comments before parse
		argv = new char *[1024];
		parse(line, argv, argNum, delimSpace); // parses for spaces
		/*for(int i = 0; i < argNum; i++)
		{
			char * pch;
			pch = strchr(line, '|');
			int tempIndex = pch-line+1;
			while(pch != NULL)
			{
				if(strchr(pch+1, '|')-line+1 == tempIndex)
				{
					parse(line, cArray.at(cArray.size()), cArray.size(); delimORs);
				}
				else if(strchr(pch+1, ';')-line+1 == tempIndex)
				{
					parse(line, cArray.at(cArray.size()), cArray.size(); delimSEMIs);
				}
				else if(strchr(pch+1], '&')-line+1 == tempIndex;
				{
					parse(line, cArray.at(cArray.size()), cArray.size(); delimANDs);
				}
			}	
		}
		for(int i = 0; i < cArray.size(); i++)
		{
			parse(*cArray.at(i), cArray.at(i), argNum, delimSpace);
		}*/
		if(argv[0] != NULL && strcmp(argv[0], "exit") == 0) // exits only on first key
		{
			cout << "Exiting the Shell... " << endl << endl;
			exit(1);
		}
		execute(argv); // executes the argument
		delete [] argv; // deletes so there is no memory leak
	}

	return 0;
}
