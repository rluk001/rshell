#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>
#include <time.h>
#include <string.h>
#include <pwd.h>
#include <vector>
#include <grp.h>
#include <ctype.h>

#define GREEN "\033[1m\033[32m"
#define BLUE "\033[1m\033[34m"
#define GRAY "\033[37m"
#define END "\033[00m"
#define GRAYBG "\033[100m"
#define CYAN "\033[1m\033[36m"
#define GRAYGREEN "\033[100m\033[1m\033[32m"
#define GRAYBLUE "\033[100m\033[1m\033[34m"

using namespace std;

int ENTERS = 0;
int STSIZE = 0;

int numDigits(int number)
{
	int digits = 0;
	if(number < 0)
	{
		digits = 1;
	}

	while(number != 0)
	{
		number /= 10;
		digits++;
	}
	return digits;
}

void printTimeCreated(struct stat &statbuf)
{
	tm * timeInfo = localtime(&(statbuf.st_mtime));
	char name[30];
	if(strftime(name, 30, "%b %d %H:%M", timeInfo) == 0)
	{
		perror("Error: strftime failed");
	}
	cout << name << " ";
}

void printWithColor(struct stat statbuf, dirent * direntp)
{
	if(statbuf.st_mode & S_IFDIR) // directory = blue
	{
		printf(BLUE"%-*s"END, (int)(strlen(direntp->d_name)), direntp->d_name); 
		cout << "  ";
	}
	else if(statbuf.st_mode & S_IXUSR) // executable = green
	{
		printf(GREEN"%-*s"END, (int)(strlen(direntp->d_name)), direntp->d_name); 
		cout << "  ";
	}
	else if(S_ISLNK(statbuf.st_mode)) // links = cyan
	{
		printf(CYAN"%-*s"END, (int)(strlen(direntp->d_name)), direntp->d_name); 
		cout << "  ";
	}
	else if(direntp->d_name[0] == '.') // hidden files = gray
	{
		printf(GRAYBG"%-*s"END, (int)(strlen(direntp->d_name)), direntp->d_name); 
		cout << "  ";
	}
	else if(direntp->d_name[0] == '.' && (statbuf.st_mode & S_IFDIR)) // hidden directory = blue with gray bg
	{
		printf(GRAYBLUE"%-*s"END, (int)(strlen(direntp->d_name)), direntp->d_name); 
		cout << "  ";
	}
	else if(direntp->d_name[0] == '.' && (statbuf.st_mode & S_IXUSR)) // hidden executable = green with gray bg
	{
		printf(GRAYGREEN"%-*s"END, (int)(strlen(direntp->d_name)), direntp->d_name); 
		cout << "  ";
	}
	else //everything else
	{
		cout << direntp->d_name << "  ";
	}	
}

void print_l(dirent* dir, struct stat & statbuf)
{
	struct passwd *pwd;
	struct group *groupID;
	if(statbuf.st_mode & S_IFDIR) //directory
	{
		cout << 'd';
	}
	else if(S_ISLNK(statbuf.st_mode)) //link
	{
		cout << 'l';
	}
	else
	{
		cout << '-';
	}
	
	if(statbuf.st_mode & S_IRUSR) 	//Owner with read permission
	{
		cout << 'r';
	}
	else
		cout << '-';
	if(statbuf.st_mode & S_IWUSR)	//Owner with write permission
	{
		cout << 'w';
	}
	else
		cout << '-';
	if(statbuf.st_mode & S_IXUSR) 	//Owner with execute permission
	{
		cout << 'x';
	}
	else
		cout << '-';
	
	if(statbuf.st_mode & S_IRGRP) 	//Group with read permission
	{
		cout << 'r';
	}
	else
		cout << '-';
	if(statbuf.st_mode & S_IWGRP) 	//Group with write permission
	{
		cout << 'w';
	}
	else
		cout << '-';
	if(statbuf.st_mode & S_IXGRP)	 //Group with execute permission
	{
		cout << 'x';
	}
	else
		cout << '-';

	if(statbuf.st_mode & S_IROTH) 	//Others with read permission
	{
		cout << 'r';
	}
	else
		cout << '-';
	if(statbuf.st_mode & S_IWOTH) 	//Others with write permission
	{
		cout << 'w';
	}
	else
		cout << '-';
	if(statbuf.st_mode & S_IXOTH) 	//Others with execute permission
	{
		cout << 'x';
	}
	else
		cout << '-';

	cout << " " << statbuf.st_nlink << " "; //# of links
	
	if(!(pwd = getpwuid(statbuf.st_uid))) // Gets the PWD if valid
	{
		perror("Error: getpwuid");
		exit(1);
	}
	cout << pwd->pw_name << " ";
	if(!(groupID = getgrgid(statbuf.st_gid))) // Gets the groupID if valid
	{
		perror("Error: getgrgid");
		exit(1);
	}
	cout << groupID->gr_name << " "; 
	
	if(numDigits(statbuf.st_size) != STSIZE)
	{
		int a = STSIZE - numDigits(statbuf.st_size);
		if(a != 0)
		{
			cout << " ";
			a--;
		}
	}
	cout << statbuf.st_size << " "; 	// prints out the size of file
	printTimeCreated(statbuf); 		// prints out time last modified
	printWithColor(statbuf, dir); 		// output
	ENTERS--;
	if(ENTERS != 0)
	{
		cout << endl;
	}
}


void output(string directory, bool a, bool l, bool R)
{
	struct stat statbuf;
	vector <string> availableLocation;
	vector <dirent *> direntPointers;
	DIR *dirp;
	int totalBlocks = 0;
	dirent *direntp;
	vector <string > dir;
	if(!(dirp = opendir(directory.c_str())))
	{
		perror("Error: opendir failed");
		exit(1);
	}
	
	while((direntp = readdir(dirp)))
	{
		if(errno != 0)
		{
			perror("Error: readdir failed");
			exit(1);
		}
		string currentDir = directory + "/" + direntp->d_name;
		char * currentDirectory = new char[currentDir.size() + 1];
		strcpy(currentDirectory, currentDir.c_str());
		if((stat(currentDirectory, &statbuf)) == -1)
		{
			perror("Error: stat failed");
			exit(1);
		}
		if(!a && (strcmp(direntp->d_name, ".") == 0))
		{
			;
		}
		else if(!a && (strcmp(direntp->d_name, "..") == 0))
		{
			;
		}
		else
		{
			totalBlocks += statbuf.st_blocks;
		}
		if(STSIZE < numDigits(statbuf.st_size))
		{
			STSIZE = numDigits(statbuf.st_size);
		}
		availableLocation.push_back(currentDir);
		direntPointers.push_back(direntp);
		ENTERS++;
		delete [] currentDirectory;
	}
	
		
	for(unsigned int i = 0; i < direntPointers.size(); i++)
	{
		for(unsigned int a = 0; a < direntPointers.size()-1; a++)
		{
			char * temp = new char [availableLocation.at(a).size() + 1];
			strcpy(temp, availableLocation.at(a).c_str());

			char * temp2 = new char [availableLocation.at(a+1).size() + 1];
			strcpy(temp2, availableLocation.at(a+1).c_str());

			for(unsigned int j = 0; temp[j]; j++)
			{
				temp[j] = tolower(temp[j]);
			}
			for(unsigned int k = 0; temp2[k]; k++)
			{
				temp2[k] = tolower(temp2[k]);
			}
			for(unsigned int h = 0; temp[h] && temp2[h]; h++)
			{
				if(temp[h] > temp2[h])
				{
					dirent * swap = direntPointers.at(a+1);
					direntPointers.at(a+1) = direntPointers.at(a);
					direntPointers.at(a) = swap;
					string stringSwap = availableLocation.at(a+1);
					availableLocation.at(a+1) = availableLocation.at(a);
					availableLocation.at(a) = stringSwap;
					break;
				}
				else if(temp[h+1] && !temp2[h+1])
				{
					dirent * swap = direntPointers.at(a+1);
					direntPointers.at(a+1) = direntPointers.at(a);
					direntPointers.at(a) = swap;
					string stringSwap = availableLocation.at(a+1);
					availableLocation.at(a+1) = availableLocation.at(a);
					availableLocation.at(a) = stringSwap;
					break;
				}
				else if(temp[h] < temp2[h])
				{
					break;
				}
			}
			delete [] temp;
			delete [] temp2;
		}
	}
	if(R)
	{
		if(strcmp(directory.c_str(), ".") == 0)
		{
			cout << ".:" << endl;
		}
		else
		{
			cout << directory << ":" << endl;
		}
	}

	if(l)
	{
		cout << "total: " << totalBlocks/2  << endl; 
	}
	for(unsigned int i = 0; i < direntPointers.size(); i++)
	{
		if((stat(availableLocation.at(i).c_str(), &statbuf)) == -1)
		{
			perror("Error: stat failed");
			exit(1);
		}
		if(!a && l && !R)	// -l flag
		{
			if(direntPointers.at(i)->d_name[0] == '.')
			{
				ENTERS--;
				continue;
			}
			print_l(direntPointers.at(i), statbuf);
		}
		else if(a && !l && !R) 	// -a flag
		{
			printWithColor(statbuf, direntPointers.at(i));
		}
		else if(!a && !l && R) 	// -R flag
		{
			if(direntPointers.at(i)->d_name[0] == '.')
			{
				continue;
			}
			
			printWithColor(statbuf, direntPointers.at(i));
			if(S_ISDIR(statbuf.st_mode))
			{
				dir.push_back(direntPointers.at(i)->d_name);
			}
		}
		else if(a && l && !R)  // -al or -la flag
		{
			print_l(direntPointers.at(i), statbuf);
		}
		else if(a && !l && R) 	// -aR or -Ra flag
		{
			printWithColor(statbuf, direntPointers.at(i));
			if(direntPointers.at(i)->d_name[0] == '.')
			{
				if(isalnum(direntPointers.at(i)->d_name[1]) == 0)
				{
					continue;
				}
			}
			if(S_ISDIR(statbuf.st_mode))
			{
				dir.push_back(direntPointers.at(i)->d_name);
			}
		}
		else if(!a && l && R) 	// -lR or -Rl flag
		{
			if(direntPointers.at(i)->d_name[0] == '.')
			{
				ENTERS--;
				continue;
			}
			print_l(direntPointers.at(i), statbuf);
		}
		else if(a && l && R) 	// -alR, -aRl, -laR, -lRa, -Ral, or -Rla flag
		{
			print_l(direntPointers.at(i), statbuf);
			if(direntPointers.at(i)->d_name[0] == '.')
			{
				if(isalnum(direntPointers.at(i)->d_name[1]) == 0)
				{
					continue;
				}
			}
			if(S_ISDIR(statbuf.st_mode))
			{
				dir.push_back(direntPointers.at(i)->d_name);
			}
		}
		else if(!a && !l && !R)	// no flags
		{
			if(direntPointers.at(i)->d_name[0] == '.')
			{
				continue;
			}
			printWithColor(statbuf, direntPointers.at(i));
		}
	}	
	cout << endl;
	unsigned int size = dir.size();
	for(unsigned int i = 0; i < size; i++)
	{
		output((directory + "/" + dir.at(i)), a, l, R);
	}

	if((closedir(dirp) == -1))
	{
		perror("Error: closedir failed");
		exit(1);
	}
	STSIZE = 0;
}
int main(int argc, char ** argv)
{
	vector <string> inputs;
	for(int i = 1; i < argc; i++)
	{
		inputs.push_back(argv[i]);
	}
	vector <string> flags;
	vector <string> directories;
	vector <int> positions;
	vector <string> failedCase;
	vector <int> failedCasePositions;
	for(unsigned int i = 0; i < inputs.size(); i++)
	{
		if(inputs.at(i).at(0) == '-')	// this means it's a flag
		{
			flags.push_back(inputs.at(i));
		}
		else if(inputs.at(i).at(0) == '/' || inputs.at(i) == "." || inputs.at(i) == ".." || inputs.at(i).find("..") != string::npos)
		{
			directories.push_back(inputs.at(i));
			positions.push_back(i);
		}
		else
		{
			failedCase.push_back(inputs.at(i));
			failedCasePositions.push_back(i);
		}
	}
	
	bool a = false, l = false , R = false;
	for(unsigned int i = 0; i < flags.size(); i++)
	{
		for(unsigned int j = 1; j < flags.at(i).size(); j++)
		{
			if(flags.at(i).at(j) == 'a') // checks if the input has the '-a' flag
			{
				a = true;
			}
			else if(flags.at(i).at(j) == 'l') // checks if the input has the '-l' flag
			{
				l = true;
			}
			else if(flags.at(i).at(j) == 'R') // checks if the input has the '-R' flag
			{
				R = true;
			}
			else
			{
				cout << "ls: invalid option -- '" << flags.at(i).at(j) << "'" << endl;
				exit(1);
			}
		}
	}

	if(directories.size() == 0)
	{
		directories.push_back(".");
		positions.push_back(failedCasePositions.size() + 1);
	}

	for(unsigned int i = 0 ; i < directories.size(); i++)
	{
		bool tF = false;
		if(failedCase.size() == 0)
		{
			output(directories.at(i), a, l, R);
			tF = true;
		}
		for(unsigned int j = 0; j < failedCase.size(); j++)
		{
			cout << "ls: cannot access " << failedCase.at(j) << ": No such file or directory" << endl;
		}
		if(directories.size() == 1 && directories.at(0) != "." && !tF)
		{
			cout << directories.at(i) << ": " << endl;
			output(directories.at(i), a, l, R);
		}
	}
	
	return 0;
}
