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
void checkForPipes(char ** argv);
bool tripString = false;

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

void checkDup(char ** argv)
{
	unsigned int inputLocation = 0, outputLocation = 0, outputOutLocation = 0;
	for(unsigned int i = 0; argv[i]; i++)
	{
		if(strcmp(argv[i], "<") == 0)
		{
			inputLocation = i;
		}
		else if(strcmp(argv[i], ">") == 0)
		{
			outputLocation = i;
		}
		else if(strcmp(argv[i], ">>") == 0)
		{
			outputOutLocation = i;
		}
	}

	for(unsigned int i = 0; argv[i]; i++)
	{
		if(strcmp(argv[i], "<") == 0)
		{
			argv[i] = 0;
			int fd = open(argv[i+1], O_RDONLY);
			if(fd == -1)
			{
				perror("Error: open failed");
				exit(1);
			}
			dup2(fd, 0);
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

			if(inputLocation != 0 && inputLocation != i)
			{
				argv[inputLocation] = 0;
				int fd = open(argv[inputLocation+1], O_RDONLY);
				if(fd == -1)
				{
					perror("Error: open failed");
					exit(1);
				}
				dup2(fd, 0);
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
			}
			else if (outputLocation != 0)
			{
				argv[outputLocation] = 0;
				int fd = open(argv[outputLocation+1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
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
			}
			else if(outputOutLocation != 0)
			{
				argv[outputOutLocation] = 0;
				int fd = open(argv[outputOutLocation+1], O_WRONLY | O_CREAT | O_APPEND, 0666);
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
			}
			return;
		}
		if(strcmp(argv[i], ">") == 0)
		{
			argv[i] = 0;
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
			if(inputLocation != 0)
			{
				argv[inputLocation] = 0;
				int fd = open(argv[inputLocation+1], O_RDONLY);
				if(fd == -1)
				{
					perror("Error: open failed");
					exit(1);
				}
				dup2(fd, 0);
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
			}
			else if (outputLocation != 0 && outputLocation != i)
			{
				argv[outputLocation] = 0;
				int fd = open(argv[outputLocation+1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
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
			}
			else if(outputOutLocation != 0)
			{
				argv[outputOutLocation] = 0;
				int fd = open(argv[outputOutLocation+1], O_WRONLY | O_CREAT | O_APPEND, 0666);
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
			}
			return;
		}

		if(strcmp(argv[i], ">>") == 0)
		{
			argv[i] = 0;
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
			if(inputLocation != 0)
			{
				argv[inputLocation] = 0;
				int fd = open(argv[inputLocation+1], O_RDONLY);
				if(fd == -1)
				{
					perror("Error: open failed");
					exit(1);
				}
				dup2(fd, 0);
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
			}
			else if (outputLocation != 0)
			{
				argv[outputLocation] = 0;
				int fd = open(argv[outputLocation+1], O_WRONLY | O_CREAT | O_TRUNC, 0666);
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
			}
			else if(outputOutLocation != 0 && outputOutLocation != i)
			{
				argv[outputOutLocation] = 0;
				int fd = open(argv[outputOutLocation+1], O_WRONLY | O_CREAT | O_APPEND, 0666);
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
			}
			return;
		}
		
		if(strcmp(argv[i], "<<<") == 0)
		{
			argv[i] = 0;
			string input;
			if(argv[i+1][0] == '\"' && argv[i+1][strlen(argv[i+1])-1] == '\"')
			{
				for(unsigned int j = 1; j < (strlen(argv[i+1])-1); j++)
				{
					input += argv[i+1][j];
				}
			}
			else
			{
				cout << "Error: invalid use" << endl;
				return;
			}
			char * buffer = new char[input.size()+1];
			strcpy(buffer, input.c_str());

			if(write(1, buffer, strlen(buffer)) == -1)
			{
				perror("Error: write");
				exit(1);
			}
			cout << endl;
			/*if(close(fd[0]) == -1)
			{
				perror("Error: close fd[0] failed");
				exit(1);
			}
			if(close(fd[1]) == -1)
			{
				perror("Error: close fd[1] failed");
				exit(1);
			}*/
			tripString = true;
			delete [] buffer;
		}
	}
}

void executeIO(char ** argv)
{
	int pid = fork();
	if(pid == -1)
	{
		perror("Error: fork failed");
		exit(1);
	}
	else if(pid > 0)
	{
		if(wait(0) == -1)
		{
			perror("Error: wait failed");
			exit(1);
		}
	}
	else if(pid == 0)
	{
		checkDup(argv);
		if(!tripString && execvp(argv[0], argv) == -1)
		{
			perror("Error: execvp failed");
			exit(1);
		}	
	}

	if(WIFEXITED(stat) && WEXITSTATUS(stat) == -1) 
	{
		return ;
	}
}

int execute(char ** argv, string OPERATORS) // Execute for regular commands
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
			exitCall = true;
			return -1;
		}
	}
	else if(pid > 0) 	// Parent process
	{
		if(wait(&stat) == -1)
		{
			perror("Error: wait has failed");
			exit(1);
		}
	}
	if(WIFEXITED(stat) && WEXITSTATUS(stat) == -1 && (OPERATORS == "&&" || OPERATORS == "||"))
	{
		return -1;
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

int parseArgs(char * line, string OPERATORS) // parseSpaces
{
	char ** parsedSpaces = parse(line, " ");
	int executeProgram = execute(parsedSpaces, OPERATORS);
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
			if(exitCall)
			{
				break;
			}
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
						exitCall = true;
						break;
					}
					if(parseArgs(parsedOr[j], "||") == 0)
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
					exitCall = true;
					break;
				}
				if(parseArgs(parsedAnd[i], "&&") != 0)
				{
					break;
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
					exitCall = true;
					break;
				}
				if(parseArgs(parsedORS[i], "||") == 0)
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
				exitCall = true;
			}
			if(parseArgs(line, "") != 0)
			{
				return ;
			}
		}
	}
}

void executeForPiping(char ** linep1, char ** linep2)
{
	int fd[2];
	bool found = false;
	unsigned int pos = 0;
	unsigned int last = 0;
	for(unsigned int i = 0; linep1[i]; i++)
	{
		if((strcmp(linep1[i], "<") == 0) || (strcmp(linep1[i], ">") == 0) || (strcmp(linep1[i], ">>") == 0) || (strcmp(linep1[i], "<<<") == 0))
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
			last = i+1;
		}
		linep1[last] = 0;
	}
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

	int tempStdIn = dup(0);
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

	checkForPipes(linep2);
	
	dup2(tempStdIn, 0);
	if(errno == -1)
	{
		perror("Error: dup2 #3 failed");
		exit(1);
	}
}

void runPipe(char ** argv, int pipeLocation, bool hasPipe)
{
	char ** linep1 = new char *[1024];
	char ** linep2 = new char *[1024];
	memset(linep1, 0, 1024);
	memset(linep2, 0, 1024);
	if(!hasPipe && pipeLocation == -1)
	{
		executeIO(argv);
	}
	else if(hasPipe && pipeLocation >= 0)
	{
		for(int i = 0; i < pipeLocation; i++)
		{
			linep1[i] = argv[i];
		}
		for(int i = pipeLocation + 1; argv[i]; i++)
		{
			linep2[i-pipeLocation-1] = argv[i];
		}
		executeForPiping(linep1, linep2);
	}
	delete [] linep1;
	delete [] linep2;
}

void checkForPipes(char ** argv)
{
	bool hasPipe = false;
	int pipeLocation = -1;
	for(unsigned int i = 0; argv[i]; i++)
	{
		if(strcmp(argv[i], "|") == 0)
		{
			hasPipe = true;
			pipeLocation = i;
			break;
		}
	}
	runPipe(argv, pipeLocation, hasPipe);
}

void parseCommands(char * line, unsigned int lineSize)
{
	bool inputR = false, outputR = false, outputRApp = false, hasPipe = false, catInput = false;
	for(unsigned int i = 0; i < lineSize + 1; i++)
	{
		if(i != 0 && line[i-1] == '<' && line[i] == '<' && line[i+1] == '<')
		{
			catInput = true;
		}
		
		if(i != 0 && line[i-1] != '<' && line[i] == '<' && line[i+1] != '<')
		{
			inputR = true;
		}

		if(line[i] == '>' && line[i+1] == '>')
		{
			outputRApp = true;
		}

		if(i != 0 && line[i-1] != '>' && line[i] == '>' && line[i+1] != '>')
		{
			outputR = true;
		}

		if(i != 0 && line[i-1] != '|' && line[i] == '|' && line[i+1] != '|')
		{
			hasPipe = true;
		}
	}

	if(hasPipe)
	{
		char ** parsedSpaces = parse(line, " ");
		checkForPipes(parsedSpaces);
		delete [] parsedSpaces;
	}
	else if(!hasPipe)
	{
		if(inputR || outputR || outputRApp || catInput)
		{
			char ** parsedSpaces = parse(line, " ");
			executeIO(parsedSpaces);
			delete [] parsedSpaces;
		}
		else if(!inputR && !outputR && !outputRApp && !catInput)
		{
			char ** parsedSemis = parse(line, ";");
			for(unsigned int i = 0; parsedSemis[i]; i++)
			{
				parseLogicOps(parsedSemis[i]);
			}
			delete [] parsedSemis;
		}
	}
}

string separateWithSpaces(string line) // Separates to make the i/o redirection in its own argv[i]
{
	string newString;
	for(unsigned int i = 0; i < line.size(); i++)
	{
		if(i != 0 && (line[i-1] != ' ' && line[i-1] != '<') && line[i] == '<' && (line[i+1] != ' ' && line[i+1] != '<'))
		{
			newString += " < ";
		}
		else if(i != 0 && (line[i-1] != ' ' && line[i-1] != '>') && line[i] == '>' && (line[i+1] != '>' && line[i+1] != ' '))
		{
			newString += " > ";
		}
		else if(i != 0 && line[i-1] != ' ' && line[i] == '>' && line[i+1] == '>' && line[i+1] != ' ')
		{
			newString += " >> ";
		}
		else if(i != 0 && (line[i] == '|' && (line[i-1] != ' ' && line[i-1] != '|') && (line[i+1] != ' ' && line[i+1] != '|')))
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
		parseCommands(line, input.size());
		delete [] line;
		if(exitCall)
		{
			break;
		}
		tripString = false;
	}
	return 0;
}
