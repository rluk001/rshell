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
	parsedWord = strtok(line, delim);
	while(parsedWord != NULL)
	{
		argv[argNum] = parsedWord;
		argNum++;
		parsedWord = strtok(NULL, delim);
	}
	argv[argNum] = NULL;
}

void execute(char ** argv)
{
	int pid = fork();
	if(pid == -1)
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

void commentCheck(char *  line)
{
	bool commCheck = false;
	int commIndex = 0;
	for(int i = 0; i < strlen(line); i++)
	{
		if(line[i] == '#')
		{
			commIndex = i;
			commCheck = true;
		}
	}
	if(commCheck)
	{
		for(int i = commIndex; i < strlen(line) - commIndex; i++)
		{
			line[i] = 0;
		}
		cout << "Comment Erased" << endl;
	}
}
int main()
{
	cout << "Initializing Shell... " << endl << endl;
		
	while(1)
	{
		printUserInfo();	
		char line[1024] = {0};
		char ** argv;
		int argNum = 0;
		char delimSpace[] = " ";
		char delimORs[] = "||";
		char delimANDs[] = "&&";
		char delimSEMis[] = ";";
		vector <char **> cArray;
		cin.getline(line, 1024);
		commentCheck(line);	
		argv = new char *[1024];
		char * orFind, semiFind, andFind;
		orFind = strchr(line, '|');
		semiFind = strchr(line, ';');
		andFind = strchr(line, '&');
		int compOR = orFind - line + 1;
		int compSEMI = semiFind - line + 1;
		int compAND = andFind - line + 1;
		while(orFind != NULL)
		{
			if(strchr(orFind+1, '|')-line+1 < compSEMI && strchr(orFind+1, '|')-line+1 < compAND)
			{
				parse(line, cArray.at(cArray.size()), cArray.size(); delimORs);
			}
			else if(strchr(semiFind+1, ';')-line+1 < compOR && strchr(semiFind+1, ';')-line+1 < compAND)
			{
				parse(line, cArray.at(cArray.size()), cArray.size(); delimSEMIs);
			}
			else if(strchr(andFind+1, '&')-line+1 < compOR && strchr(andFind+1, '&')-line+1 < compSEMI)
			{
				parse(line, cArray.at(cArray.size()), cArray.size(); delimANDs);
			}
		}	
		for(int i = 0; i < cArray.size(); i++)
		{
			parse(*cArray.at(i), cArray.at(i), argNum, delimSpace);
		}
		for(int j = 0; j < cArray.size(); j++)
		{
			if(cArray.at(0) != NULL && strcmp(cArray.at(0), "exit") == 0)
			{
				cout << "Exiting the Shell... " << endl << endl;
				exit(1);
			}
		}
		for(int i = 0; i < cArray.size(); i++)
		{
			execute(cArray.at(i));
		}
		delete [] argv;
	}

	return 0;
}
