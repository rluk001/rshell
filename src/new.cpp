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
#include <string.h>
#include <pwd.h>
#include <vector>
using namespace std;

void executeForPiping(char ** argv1, char ** argv2, bool ampersand);

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

void checkDup(char ** argv)
{
	for(unsigned int i = 0; argv[i]; i++)
	{
		if(strcmp(argv[i], "<") == 0)
		{
			argv[i] = 0; // to get rid of '<' sign
			int fd = open(argv[i+1], O_RDONLY);
			if(fd == -1)
			{
				perror("Error: open failed");
				exit(1);
			}
			dup2(fd, 0);
			if(errno == -1)
			{
				perror("There was an error with dup2");
				exit(1);
			}
			if(close(fd) == -1)
			{
				perror("Error: close failed");
				exit(1);
			}
			return ;
		}
		else if(strcmp(argv[i], ">") == 0)
		{
			argv[i] = 0; // to get rid of > sign;
			int fd = open(argv[i+1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
			if(fd == -1)
			{
				perror("Error: open failed");
				exit(1);
			}
			dup2(fd, 1);
			if(errno == -1)
			{
				perror("Error: dup2 failed");
				exit(1);
			}
			if(close(fd) == -1)
			{
				perror("Error: close failed");
				exit(1);
			}
			return;
		}
		else if(strcmp(argv[i], ">>") == 0)
		{
			argv[i] = 0; // to get rid of ">>" sign
			int fd = open(argv[i+1], O_WRONLY | O_CREAT | O_APPEND, 0666);
			if(fd == -1)
			{
				perror("Error: open failed");
				exit(1);
			}
			dup2(fd, 1);
			if(errno == -1)
			{
				perror("Error: dup2 failed");
				exit(1);
			}
			if(close(fd) == -1)
			{
				perror("Error: close failed");
				exit(1);
			}
			return;
		}
	}
}
			
void executeIO(char ** argv, bool ampersand)
{
	int pid = fork();

	if(pid == -1)
	{
		perror("Error: fork failed");
		exit(1);
	}
	else if(pid > 0)
	{
		if(!ampersand)
		{
			if(wait(0) == -1)
			{
				perror("Error: wait failed");
				exit(1);
			}
		}
	}
	else if(pid == 0)
	{
		checkDup(argv);
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

string separateWithSpaces(string line)
{
	string newString;
	for(unsigned int i = 0; i < line.size(); i++)
	{
		if(i != 0 && line[i-1] != ' ' && line[i] == '<' && line[i+1] != ' ')
		{
			newString += " < ";
		}
		else if(i != 0 && line[i-1] != ' ' && line[i] == '>' && line[i+1] != ' ')
		{
			newString += " > ";
		}
		else if(i != 0 && line[i-1] != ' ' && line[i] == '>' && line[i+1] == '>' && line[i+2] != ' ')
		{
			newString += " >> ";
		}
		else if(i != 0 && (line[i] == '|' && line[i-1] != ' ' && line[i+1] != ' '))
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

void checkForPipes(char ** line, bool ampersand)
{
	bool hasPipe = false;
	int pipeLocation = 0;

	char ** linep1;
	linep1 = new char *[1024];

	char ** linep2;
	linep2 = new char *[1024];

	for(unsigned int i = 0; line[i]; i++)
	{
		if(strcmp(line[i], "|") == 0)
		{
			hasPipe = true;
			pipeLocation = i;
			break;
		}
	}
	
	if(!hasPipe)
	{
		executeIO(line, ampersand);
	}

	else if(hasPipe)
	{
		for(int i = 0; i < pipeLocation; i++)
		{
			linep1[i] = line[i];
		}
		
		linep1[pipeLocation] = NULL;
	
		int a = 0;
		for(int i = pipeLocation+1; line[i]; i++)
		{
			linep2[a] = line[i];
			a++;
		}
		linep2[a] = NULL;

		executeForPiping(linep1, linep2, ampersand);
	}

	delete [] linep1;
	delete [] linep2;
}
		
void executeForPiping(char ** linep1, char ** linep2, bool ampersand)
{
	int fd[2];
	bool found = false;
	unsigned int pos;
	if(pipe(fd) == -1)
	{
		perror("Error: pipe failed");
		exit(1);
	}
	
	int pid = fork();
	if(pid < 0)
	{
		perror("Error: fork failed");
		exit(1);
	}
	else if(pid == 0)
	{
		dup2(fd[1], 1);
		if(errno == -1)
		{
			perror("Error: dup2 failed");
			exit(1);
		}
		close(fd[0]);
		if(errno == -1)
		{
			perror("Error: close(1) failed");
			exit(1);
		}
		/*for(unsigned int i = 0; linep1[i]; i++)
		{
			if((strcmp(linep1[i], "<") == 0) || (strcmp(linep1[i], ">") == 0) || (strcmp(linep1[i], ">>") == 0))
			{
				linep1[i] = 0;
				pos = i;
				found = true;
			}
		}
		if(found)
		{
			for(unsigned int i = pos; linep1[i+1]; i++)
			{
				linep1[i] = linep1[i+1];
			}
			for(unsigned int i = 0; linep1[i]; i++)
			{
				cout << linep1[i] << " ";
			}
		}*/
		if(execvp(linep1[0], linep1) == -1)
		{
			perror("Error: execvp failed");
			exit(1);
		}
	}
	close(fd[1]);
	if(errno == -1)
	{
		perror("Error: close failed");
		exit(1);
	}

	int tempStdIn = dup(0); // save for stdin
	if(tempStdIn == -1)
	{
		perror("Error: dup failed");
		exit(1);
	}
	dup2(fd[0], 0);
	if(errno == -1)
	{
		perror("Error: dup2 #2 failed");
		exit(1);
	}
	if(wait(0) == -1)
	{
		perror("Error: wait failed");
		exit(1);
	}

	checkForPipes(linep2, ampersand);

	dup2(tempStdIn, 0);
	if(errno == -1)
	{
		perror("Error: dup2 #3 failed");
		exit(1);
	}
}

void parseCommands(char * line, unsigned int lineSize, bool ampersand)
{
	bool inputR = false, outputR = false, outputRApp = false, hasPipe = false;
	for(unsigned int i = 0; i < lineSize; i++)
	{
		if(line[i] == '<')  // '<'
		{
			inputR = true;
		}
		if(i != 0 && line[i-1] != '>' && line[i] == '>' && line[i+1] != '>') // '>'
		{
			outputR = true;
		}
		if(line[i] == '>' && line[i+1] == '>') // '>>'
		{
			outputRApp = true;
		}
		if(line[i] == '|' && line[i+1] != '|')
		{
			hasPipe = true;
		}
		/*if(line[i-1] == '<' && line[i] == '<' && line[i+1] == '<') // '<<<'
		{
			inputRString = true;
			pos5 = i;
		}*/
	}
	if(hasPipe)
	{
		char ** parsedSpaces = parse(line, " ");
		checkForPipes(parsedSpaces, ampersand);
		for(int i = 0; parsedSpaces[i] != NULL; i++)
		{
			delete parsedSpaces[i];
		}
		delete parsedSpaces;
	}
	else if(!hasPipe)
	{
		if(inputR)
		{
			char ** parsedSpaces = parse(line, " ");
			executeIO(parsedSpaces, ampersand);	
			for(int i = 0; parsedSpaces[i] != NULL; i++)
			{
				delete parsedSpaces[i];
			}
			delete parsedSpaces;
		}
		else if(outputR)
		{
			char ** parsedSpaces = parse(line, " ");
			executeIO(parsedSpaces, ampersand);
			for(int i = 0; parsedSpaces[i] != NULL; i++)
			{
				delete parsedSpaces[i];
			}
			delete parsedSpaces;
		}
		else if(outputRApp)
		{
			char ** parsedSpaces = parse(line, " ");
			executeIO(parsedSpaces, ampersand);
			for(int i = 0; parsedSpaces[i] != NULL; i++)
			{
				delete parsedSpaces[i];
			}
			delete parsedSpaces;
		}
		/*else if(inputRString)
		{

		}*/
		else
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
	}
}
	
int main()
{
	cout << "Initializing Shell... " << endl << endl;
	string input;
	char * line;
	do
	{
		bool ampersand = false;
		printUserInfo();	
		getline(cin, input);
		commentCheck(input);	
		input = separateWithSpaces(input);
		cout << input << endl;
		line = new char[input.size()+1];
		strcpy(line, input.c_str());
		if(strcmp(line, "exit") == 0)
		{
			cout << "Exiting the shell ..." << endl;
			exit(1);
		}
		parseCommands(line, input.size(), ampersand);
		delete line;
	}while(1);	
	return 0;
}
