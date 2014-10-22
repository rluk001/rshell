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
void parse(char * line, char ** argv, char * delim)
{	
	int i = 0;
	char *parsedWord;
	parsedWord = strtok(line, delim);
	while(parsedWord != NULL)
	{
		argv[i] = parsedWord;
		i++;
		parsedWord = strtok(NULL, delim);
	}
	cout << i << endl;
	argv[i] = NULL;
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
		char delimSpace[] = " ";
		char delimORs[] = "||";
		char delimANDs[] = "&&";
		char delimSEMIs[] = ";";
		vector <char **> cArray;
		cin.getline(line, 1024);
		commentCheck(line);	
		argv = new char *[1024];
		char * orFind, * semiFind, * andFind;
		orFind = strchr(line, '|');
		semiFind = strchr(line, ';');
		andFind = strchr(line, '&');
		bool orTF = false, semiTF = false, andTF = false;
		int compOR = 0, compSEMI = 0, compAND = 0;
		if(orFind != NULL)
		{
			compOR = orFind - line + 1;
			orTF = true;
		}
		cout << orTF << endl;
		if(semiFind != NULL)
		{
			compSEMI = semiFind - line + 1;
			semiTF = true;
		}
		cout << semiTF << endl;
		if(andFind != NULL)
		{
			compAND = andFind - line + 1;
			andTF = true;
		}
		cout << andTF << endl;
		while(orFind != NULL || semiFind != NULL || andFind != NULL)
		{
			if(strchr(orFind+1, '|')-line+1 < compSEMI && strchr(orFind+1, '|')-line+1 < compAND && orFind)
			{
				parse(line, cArray.at(cArray.size()), delimORs);
				orTF = false;
			}
			else if(strchr(semiFind+1, ';')-line+1 < compOR && strchr(semiFind+1, ';')-line+1 < compAND && semiFind)
			{
				parse(line, cArray.at(cArray.size()), delimSEMIs);
				semiTF = false;
			}
			else if(strchr(andFind+1, '&')-line+1 < compOR && strchr(andFind+1, '&')-line+1 < compSEMI && andFind)
			{
				parse(line, cArray.at(cArray.size()), delimANDs);
				andTF = false;
			}
		}
		cout << "Passed Through the AND/OR/SEMIS" << endl;	
		for(int i = 0; i < 2 ; i++)
		{	
			
			cout << "out range" << endl;
			parse(*cArray.at(i), cArray.at(i), delimSpace);
		}
		cout << "Out of range" << endl;
		for(int j = 0; j < cArray.size(); j++)
		{
			if(*cArray.at(0) != NULL && strcmp(*cArray.at(0), "exit") == 0)
			{
				cout << "Exiting the Shell... " << endl << endl;
				exit(1);
			}
		}
		for(int i = 0; i < cArray.size(); i++)
		{
			execute(cArray.at(i));
		}
		cout << "Went through entire code" << endl;
		delete [] argv;
	}

	return 0;
}
