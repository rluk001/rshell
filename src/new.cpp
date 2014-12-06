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
#include <signal.h>
using namespace std;

int processID = 0;

void executeForPiping(char ** argv1, char ** argv2, bool ampersand);
void runPipe(char ** line, bool ampersand, int pipeLocation, bool hasPipe);
char * findPath(const char * pathName);
void myExecVp(char ** argv);

void printC(int sig)
{
	if(signal(SIGINT, SIG_IGN) == SIG_ERR)
	{
		perror("Error: signal failed");
		cout << sig << endl;
		exit(1);
	}
}

void printZ(int sig)
{
	if(signal(SIGTSTP, SIG_DFL) == SIG_ERR)
	{
		perror("Error: signal CTRL-Z with SIG_DFL failed");
		exit(1);
	}
	
	if(kill(getpid(), SIGTSTP) == -1)
	{
		perror("Error: kill failed");
		cout << sig << endl;
		exit(1);
	}
}

void printUserInfo() // Prints pwname and hostname
{
	char *login = getpwuid(getuid())->pw_name;
	if(login == NULL)
	{
		perror("Error: getpwuid failed");
		exit(1);
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

void printCurrentWorkingDirectory()
{
	char buf[1024];
	memset(buf, 0, 1024);
	if(getcwd(buf, 1024) == NULL)
	{
		perror("Error: getcwd failed");
		exit(1);
	}
	else if(getcwd(buf, 1024) != NULL)
	{
		cout << buf << endl;
	}
}
int execute(char ** argv) // Execute for regular commands
{	
	int stat = 0;
	int pid = fork();
	processID = pid;
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
		if(signal(SIGINT, SIG_IGN) == SIG_ERR)
		{
			perror("Error: signal CTRL-C failed");
			exit(1);
		}
		if(signal(SIGTSTP, SIG_IGN) == SIG_ERR)
		{
			perror("Error: signal CTRL-Z failed");
			exit(1);
		}
	}
	else if(pid == 0)	// Child process (exec)
	{
		if(signal(SIGINT, printC) == SIG_ERR)
		{
			perror("Error: signal CTRL-C failed");
			exit(1);
		}
		if(signal(SIGTSTP, printZ) == SIG_ERR)
		{
			perror("Error: signal CTRL-Z failed");
			exit(1);
		}
		execvp(argv[0], argv);
		//myExecVp(argv);
	}
	return stat;
}

void checkDup(char ** argv)
{
	unsigned int inputLocation = 0, outputLocation = 0, outputoutLocation = 0;
	for(unsigned int i = 0; argv[i]; i++) // checks if there are more than one i/o redirection
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
			outputoutLocation = i;
		}
	}
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
			if(inputLocation != 0 && inputLocation != i)
			{
				argv[inputLocation] = 0; // to get rid of '<' sign
				int fd = open(argv[inputLocation+1], O_RDONLY);
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
			}
			else if(outputLocation != 0)
			{
				argv[outputLocation] = 0; // to get rid of > sign;
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
			else if(outputoutLocation != 0)
			{
				argv[outputoutLocation] = 0; // to get rid of ">>" sign
				int fd = open(argv[outputoutLocation+1], O_WRONLY | O_CREAT | O_APPEND, 0666);
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
			return ;
		}
		if(strcmp(argv[i], ">") == 0)
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
			if(inputLocation != 0)
			{
				argv[inputLocation] = 0; // to get rid of '<' sign
				int fd = open(argv[inputLocation+1], O_RDONLY);
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
			}
			else if(outputLocation != 0 && outputLocation != i)
			{
				argv[outputLocation] = 0; // to get rid of > sign;
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
			else if(outputoutLocation != 0)
			{
				argv[outputoutLocation] = 0; // to get rid of ">>" sign
				int fd = open(argv[outputoutLocation+1], O_WRONLY | O_CREAT | O_APPEND, 0666);
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
			return ;
		}
		if(strcmp(argv[i], ">>") == 0)
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
			if(inputLocation != 0)
			{
				argv[inputLocation] = 0; // to get rid of '<' sign
				int fd = open(argv[inputLocation+1], O_RDONLY);
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
			}
			else if(outputLocation != 0)
			{
				argv[outputLocation] = 0; // to get rid of > sign;
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
			else if(outputoutLocation != 0 && outputoutLocation != i)
			{
				argv[outputoutLocation] = 0; // to get rid of ">>" sign
				int fd = open(argv[outputoutLocation+1], O_WRONLY | O_CREAT | O_APPEND, 0666);
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
			return ;
		}
	}
}
			
void executeIO(char ** argv, bool ampersand)
{
	int pid = fork();
	processID = pid;
	if(pid == -1)
	{
		perror("Error: fork failed");
		exit(1);
	}
	else if(pid > 0)
	{
		if(!ampersand) // ampersand is for executing background in a subshell
		{
			if(wait(0) == -1)
			{
				perror("Error: wait failed");
				exit(1);
			}
			if(signal(SIGINT, SIG_IGN) == SIG_ERR)
			{
				perror("Error: signal CTRL-C failed");
				exit(1);
			}
			if(signal(SIGTSTP, SIG_IGN) == SIG_ERR)
			{
				perror("Error: signal CTRL-Z failed");
				exit(1);
			}
		}
	}
	else if(pid == 0)
	{
		if(signal(SIGINT, printC) == SIG_ERR)
		{
			perror("Error: signal CTRL-C failed");
			exit(1);
		}
		if(signal(SIGTSTP, printZ) == SIG_ERR)
		{
			perror("Error: signal CTRL-Z failed");
			exit(1);
		}
		checkDup(argv); // Dup for i/o redirection
		execvp(argv[0], argv);
		//myExecVp(argv);
	}
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
		memset(argvCount[i], 0, cArray.size() + 1);
		strcpy(argvCount[i], cArray[i]);
	}

	return argvCount;
}

int parseArgs(char * line) // parseSpaces
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
	char * orFind = strchr(line, '|'); // Find ORS
	char * andFind = strchr(line, '&'); // Find AND
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
	delete [] parsedTokens;	
}

string separateWithSpaces(string line) // Separates to make the i/o redirection in its own argv[i]
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

void checkForPipes(char ** line, bool ampersand)
{
	bool hasPipe = false;
	int pipeLocation = -1;
	unsigned int i = 0;
	while(line[i])
	{
		if(strcmp(line[i], "|") == 0)
		{
			hasPipe = true;
			pipeLocation = i;
			break;
		}
		i++;
	}
	runPipe(line, ampersand, pipeLocation, hasPipe);
}

void runPipe(char ** line, bool ampersand, int pipeLocation, bool hasPipe)
{
	char ** linep1 = new char *[1024];
	char ** linep2 = new char *[1024];
	memset(linep1, 0, 1024);
	memset(linep2, 0, 1024);
	if(!hasPipe && pipeLocation == -1) // if it doesnt have pipe
	{
		executeIO(line, ampersand);
	}
	else if(hasPipe && pipeLocation >= 0) // if it has pipe
	{
		for(int i = 0; i < pipeLocation; i++)
		{
			linep1[i] = line[i];
		}
		for(int i = pipeLocation+1; line[i]; i++)
		{
			linep2[i-pipeLocation-1] = line[i];
		}
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
	unsigned int last;
	for(unsigned int i = 0; linep1[i]; i++) // check for the first part of the pipe
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
	processID = pid;
	if(pid < 0)
	{
		perror("Error: fork failed");
		exit(1);
	}
	else if(pid > 0)
	{
		if(wait(0) == -1)
		{
			perror("Error: wait piping failed");
			exit(1);
		}
		if(signal(SIGINT, SIG_IGN) == SIG_ERR)
		{
			perror("Error: signal CTRL-C failed");
			exit(1);
		}
		if(signal(SIGTSTP, SIG_IGN) == SIG_ERR)
		{
			perror("Error: signal CTRL-Z failed");
			exit(1);
		}
	}
	else if(pid == 0)
	{
		if(signal(SIGINT, printC) == SIG_ERR)
		{
			perror("Error: signal CTRL-C failed");
			exit(1);
		}
		if(signal(SIGTSTP, printZ) == SIG_ERR)
		{
			perror("Error: signal CTRL-Z failed");
			exit(1);
		}
		dup2(fd[1], 1);
		if(errno == -1)
		{
			perror("Error: dup2 failed");
			exit(1);
		}
		close(fd[0]); // close stdin
		if(errno == -1)
		{
			perror("Error: close(1) failed");
			exit(1);
		}
		execvp(linep1[0], linep1);
		//myExecVp(linep1);
	}
	close(fd[1]); // close stdout
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

	checkForPipes(linep2, ampersand); // recursive needed for pipe chaining

	dup2(tempStdIn, 0);
	if(errno == -1)
	{
		perror("Error: dup2 #3 failed");
		exit(1);
	}
}

void cdFunction(char ** argv, int numberOfArgs)
{
	if(numberOfArgs == 1)
	{
		char * home = findPath("HOME");
		if(chdir(home) == -1)
		{
			perror("Error: chdir(home) failed");
			exit(1);
		}
	}
	else
	{
		if(chdir(argv[1]) == -1)
		{
			perror("Error: chdir(argv[1]) failed");
			exit(1);
		}
	}
	printCurrentWorkingDirectory();
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
		if((line[i] == '|' && line[i+1] != '|') || (line[i] != '|' && line[i+1] == '|')) // '|'
		{
			hasPipe = true;
		}
	}
	if(hasPipe) // if hasPipe
	{
		char ** parsedSpaces = parse(line, " ");
		checkForPipes(parsedSpaces, ampersand);
		for(int i = 0; parsedSpaces[i] != NULL; i++)
		{
			delete parsedSpaces[i];
		}
		delete parsedSpaces;
	}
	else if(!hasPipe) // if there's no pipe
	{
		char * line2 = new char [lineSize];
		strcpy(line2, line);
		char ** parsedStuff = parse(line2, " ");
		int numberOfArgs = 0;
		if(strcmp(parsedStuff[0], "cd") == 0)
		{
			for(unsigned int i = 0; parsedStuff[i]; i++)
			{
				numberOfArgs++;
			}	
			cdFunction(parsedStuff, numberOfArgs);
		}
		else if(inputR || outputR || outputRApp) // if its any i/o redirection
		{
			char ** parsedSpaces = parse(line, " ");
			executeIO(parsedSpaces, ampersand);	
			for(int i = 0; parsedSpaces[i] != NULL; i++)
			{
				delete parsedSpaces[i];
			}
			delete parsedSpaces;
		}
		else if(strcmp(parsedStuff[0], "fg") == 0)
		{
			if(processID == 0)
			{
				cout << "fg: current: no such job" << endl;
			}
			else
			{
				kill(processID, SIGCONT);
			}
		}
		else if(strcmp(parsedStuff[0], "bg") == 0)
		{
			if(processID == 0)
			{
				cout << "bg: current: no such job" << endl;
			}
			else
			{
				kill(processID, SIGCONT);
				processID = 0;
			}
		}
		else if(!inputR && !outputR && !outputRApp) // regular commands
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

		delete [] line2;
	}
}

char ** parsePath(char * paths, const char *delim) // parse path by delimiter
{
	char ** finalPaths = new char * [1024];
	int i = 0;
	finalPaths[i] = strtok(paths, delim);
	for( ; paths[i]; i++)
	{
		finalPaths[i] = strtok(NULL, delim);
	}
	return finalPaths;
}

void myExecVp(char ** argv)
{
	char * allPaths = findPath("PATH");
	char ** parsedPaths = parse(allPaths, ":");
	for(unsigned int i = 0; parsedPaths[i]; i++)
	{
		if(parsedPaths[i][strlen(parsedPaths[i])-1] != '/')
		{
			strcat(parsedPaths[i], "/");
		}
		strcat(parsedPaths[i], argv[0]);
		
		char * argv2[1024] = {0};
		argv2[0] = parsedPaths[i];
		for(unsigned int j = 1; argv[j]; j++)
		{
			argv2[j] = argv[j];
		}
		
		if(execv(argv2[0], argv2) == -1)
		{
			//perror("Error: execv failed");
			//exit(1);
		}
	}
	if(errno != 0)
	{
		perror("Error: execv failed");
		exit(1);
	}
	//delete [] parsedPaths;
}

char * findPath(const char * pathName)
{
	char * path = getenv(pathName);
	if(path == NULL)
	{
		perror("Error: getenv failed");
		exit(1);
	}
	return path;
}
int main()
{
	cout << "Initializing Shell... " << endl;
	string input;
	char * line = {0};
	while(1)
	{
		if(signal(SIGINT, printC) == SIG_ERR)
		{
			perror("Error: signal CTRL-C failed");
			exit(1);
		}
		if(signal(SIGTSTP, printZ) == SIG_ERR)
		{
			perror("Error: signal CTRL-Z failed");
			exit(1);
		}
		bool ampersand = false;
		printUserInfo();	
		getline(cin, input);
		commentCheck(input);	
		while(input == "")
		{
			printUserInfo();
			getline(cin, input);
		}
		input = separateWithSpaces(input);
		line = new char[input.size()+1];
		strcpy(line, input.c_str());
		if(strcmp(line, "exit") == 0)
		{
			cout << "Exiting the shell ..." << endl;
			exit(1);
		}
		parseCommands(line, input.size(), ampersand);
		delete [] line;
	}
	return 0;
}
