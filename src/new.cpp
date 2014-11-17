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

int executeWithIO(char ** argv, char * line)
{
	int stat = 0;
	int pid = fork();

	if(pid == -1)
	{
		perror("Error: fork failed");
		exit(1);
	}
	else if(pid > 0)
	{
		if(wait(&stat) == -1)
		{
			perror("Error: wait has failed");
			exit(1);
		}
	}
	else if(pid == 0)
	{
		int fd = open(line, O_RDWR, 0666);

		if(fd == -1)
		{
			perror("Error: open failed");
			exit(1);
		}

		if(close(1) == -1)
		{
			perror("Error: close failed");
			exit(1);
		}

		dup(fd);
		if(errno == -1)
		{
			perror("Error; dup failed");
			exit(1);
		}

		if(execvp(argv[0], argv) == -1)
		{
			perror("Error: execvp failed");
			exit(1);
		}
	}
}
void commentCheck(string & input)
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
	
	for(unsigned int i = 0; i < cArray.size(); i++)
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
			if(andTF)
			{
				if((parseArgs(parsedTokens[i])) != 0)
				{
					return;
				}
			}
			else if(!andTF)
			{
				if(parseArgs(parsedTokens[i]) == 0)
				{
					return;	
				}
			}
		}
		else if(!orTF)
		{
			if(andTF)
			{
				if(parseArgs(parsedTokens[i]) != 0)
				{
					return;
				}
			}
			if(!andTF)
			{
				if((parseArgs(parsedTokens[i])) == 0)
				{
					return;
				}
			}
		}
	}
	for(int i = 0; parsedTokens[i] != NULL; i++)
	{
		delete parsedTokens[i];
	}
	delete parsedTokens;	
}

void parseCommands(char * line, int lineSize)
{
	int pos1 = -1, pos2 = -1, pos3 = -1, pos4 = -1, pos5 = -1;
	bool inputR = false, outputR = false, outputRApp = false, pipe = false, inputRString = false;
	for(unsigned int i = 0; line[i+1]; i++)
	{
		if(line[i] == '<' && line[i+1] != '<')
		{
			inputR = true;
			pos1 = i;
		}
		else if(line[i-1] != '>' && line[i] == '>' && line[i+1] != '>')
		{
			outputR = true;
			pos2 = i;
		}
		else if(line[i] == '>' && line[i+1] == '>')
		{
			outputRApp = true;
			pos3 = i;
		}
		else if(line[i-1] != '|' && line[i] == '|' && line[i+1] != '|')
		{
			pipe = true;
			pos4 = i;
		}
		else if(line[i-1] == '<' && line[i] == '<' && line[i+1] == '<')
		{
			inputRString = true;
			pos5 = i;
		}
	}
	if(inputR)
	{
		char * linep1 = new char [pos1 + 1];
		char * linep2 = new char [lineSize - pos1 + 1];
		for(unsigned int i = 0; i < pos1; i++)
		{
			linep1[i] = line[i];
		}
		linep1[pos1] == '\0';
		int a = 0;
		for(unsigned int j = pos1 + 1; j < lineSize; j++)
		{
			linep2[a] = line[j];
			a++;
		}
		linep2[lineSize - pos1] = '\0';
		
		char ** parsedSemis = parse(linep1, ";");
		for(unsigned int i = 0; parsedSemis[i]; i++)
		{
			parseLogicOps(parsedSemis[i]);
		}
		for(unsigned int i = 0; parsedSemis[i]; i++)
		{
			delete parsedSemis[i];
		}
		delete [] parsedSemis;

		
		char ** parsedSemis2 = parse(line, ";");
		for(unsigned int i = 0; parsedSemis2[i]; i++)
		{
			parseLogicOps(parsedSemis2[i]);
		}
		for(unsigned int i = 0; parsedSemis2[i]; i++)
		{
			delete parsedSemis2[i];
		}
		delete [] parsedSemis2;
	}
	else if(outputR)
	{
		char * linep1 = new char [pos2 + 1];
		char * linep2 = new char [lineSize - pos2 + 1];
		for(unsigned int i = 0; i < pos2; i++)
		{
			linep1[i] = line[i];
		}
		linep1[pos2] == '\0';
		int a = 0;
		for(unsigned int j = pos2 + 1; j < lineSize; j++)
		{
			linep2[a] = line[j];
			a++;
		}
		linep2[lineSize - pos2] = '\0';

		char ** parsedSemis = parse(linep1, ";");
		for(unsigned int i = 0; parsedSemis[i]; i++)
		{
			parseLogicOps(parsedSemis[i]);
		}
		for(unsigned int i = 0; parsedSemis[i]; i++)
		{
			delete parsedSemis[i];
		}
		delete [] parsedSemis;

		
		char ** parsedSemis2 = parse(line, ";");
		for(unsigned int i = 0; parsedSemis2[i]; i++)
		{
			parseLogicOps(parsedSemis2[i]);
		}
		for(unsigned int i = 0; parsedSemis2[i]; i++)
		{
			delete parsedSemis2[i];
		}
		delete [] parsedSemis2;
	}
	else if(outputRApp)
	{
		char * linep1 = new char [pos3 + 1];
		char * linep2 = new char [lineSize - pos3 + 1];
		for(unsigned int i = 0; i < pos3; i++)
		{
			linep1[i] = line[i];
		}
		linep1[pos3] == '\0';
		int a = 0;
		for(unsigned int j = pos3 + 1; j < lineSize; j++)
		{
			linep2[a] = line[j];
			a++;
		}
		linep2[lineSize - pos3] = '\0';
		
		char ** parsedSemis = parse(linep1, ";");
		for(unsigned int i = 0; parsedSemis[i]; i++)
		{
			parseLogicOps(parsedSemis[i]);
		}
		for(unsigned int i = 0; parsedSemis[i]; i++)
		{
			delete parsedSemis[i];
		}
		delete [] parsedSemis;

		
		char ** parsedSemis2 = parse(line, ";");
		for(unsigned int i = 0; parsedSemis2[i]; i++)
		{
			parseLogicOps(parsedSemis2[i]);
		}
		for(unsigned int i = 0; parsedSemis2[i]; i++)
		{
			delete parsedSemis2[i];
		}
		delete [] parsedSemis2;
	}
	else if(pipe)
	{
		char * linep1 = new char [pos4 + 1];
		char * linep2 = new char [lineSize - pos4 + 1];
		for(unsigned int i = 0; i < pos4; i++)
		{
			linep1[i] = line[i];
		}
		linep1[pos4] == '\0';
		int a = 0;
		for(unsigned int j = pos4 + 1; j < lineSize; j++)
		{
			linep2[a] = line[j];
			a++;
		}
		linep2[lineSize - pos4] = '\0';	
		
		char ** parsedSemis = parse(linep1, ";");
		for(unsigned int i = 0; parsedSemis[i]; i++)
		{
			parseLogicOps(parsedSemis[i]);
		}
		for(unsigned int i = 0; parsedSemis[i]; i++)
		{
			delete parsedSemis[i];
		}
		delete [] parsedSemis;

		
		char ** parsedSemis2 = parse(line, ";");
		for(unsigned int i = 0; parsedSemis2[i]; i++)
		{
			parseLogicOps(parsedSemis2[i]);
		}
		for(unsigned int i = 0; parsedSemis2[i]; i++)
		{
			delete parsedSemis2[i];
		}
		delete [] parsedSemis2;
	}
	else if(inputRString)
	{
		char * linep1 = new char [pos5 + 1];
		char * linep2 = new char [lineSize - pos5 + 1];
		for(unsigned int i = 0; i < pos5; i++)
		{
			linep1[i] = line[i];
		}
		linep1[pos5] == '\0';
		int a = 0;
		for(unsigned int j = pos5 + 1; j < lineSize; j++)
		{
			linep2[a] = line[j];
			a++;
		}
		linep2[lineSize - pos5] = '\0';

		char ** parsedSemis = parse(linep1, ";");
		for(unsigned int i = 0; parsedSemis[i]; i++)
		{
			parseLogicOps(parsedSemis[i]);
		}
		for(unsigned int i = 0; parsedSemis[i]; i++)
		{
			delete parsedSemis[i];
		}
		delete [] parsedSemis;

		
		char ** parsedSemis2 = parse(line, ";");
		for(unsigned int i = 0; parsedSemis2[i]; i++)
		{
			parseLogicOps(parsedSemis2[i]);
		}
		for(unsigned int i = 0; parsedSemis2[i]; i++)
		{
			delete parsedSemis2[i];
		}
		delete [] parsedSemis2;
	}
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
	int lineSize = 0;
	char * line;
	do
	{
		printUserInfo();	
		getline(cin, input);
		commentCheck(input);
		line = new char[input.size()+1];
		strcpy(line, input.c_str());
		lineSize = input.size()+1;
		
		if(strcmp(line, "exit") == 0)
		{
			cout << "Exiting the shell ..." << endl;
			exit(1);
		}

		parseCommands(line, lineSize);
		delete line;
	}while(1);	
	return 0;
}
