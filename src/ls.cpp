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

#define GREEN "\033[1m\033[32m"
#define BLUE "\033[1m\033[34m"
#define GRAY "\033[00m"
#define GRAYF "\033[47m"
#define CYAN "\033[1m\033[36m"
#define GRAYGREEN "\033[47m\033[1m\033[32m"
#define GRAYBLUE "\033[47m\033[1m\033[34m"

using namespace std;

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
		cout << BLUE << direntp->d_name << GRAY << " ";
	}
	else if(statbuf.st_mode & S_IXUSR) // executable = green
	{
		cout << GREEN << direntp->d_name << GRAY << " ";
	}
	else if(direntp->d_name[0] == '.') // hidden files = gray
	{
		cout << GRAY << direntp->d_name << GRAY << " ";
	}
	else if(direntp->d_name[0] == '.' && (statbuf.st_mode & S_IFDIR)) // hidden directory = blue with gray bg
	{
		cout << GRAYBLUE << direntp->d_name << GRAY << " ";
	}
	else if(direntp->d_name[0] == '.' && (statbuf.st_mode & S_IXUSR)) // hidden executable = green with gray bg
	{
		cout << GRAYGREEN << direntp->d_name << GRAY << " ";
	}
	else //everything else
	{
		cout << direntp->d_name << " ";
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
	else if(statbuf.st_mode & S_IFLNK) //link
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
	
	cout << statbuf.st_size << " "; 	// prints out the size of file
	printTimeCreated(statbuf); 		// prints out time last modified
	printWithColor(statbuf, dir); 		// output
	cout << endl;
}
void output(string directory, bool a, bool l, bool R)
{
	struct stat statbuf;
	
	DIR *dirp;
	dirent *direntp;
	vector <string > dir;
	if(!(dirp = opendir(directory.c_str())))
	{
		perror("Error: opendir failed");
		exit(1);
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
		//statVect.push_back(stat(currentDirectory, &statbuf));
		if(!a && l && !R)	// -l flag
		{
			if(direntp->d_name[0] == '.')
			{
				continue;
			}
			print_l(direntp, statbuf);
		}
		else if(a && !l && !R) 	// -a flag
		{
			printWithColor(statbuf, direntp);
		}
		else if(!a && !l && R) 	// -R flag
		{
			if(direntp->d_name[0] == '.')
			{
				continue;
			}
			
			printWithColor(statbuf, direntp);
			if(S_ISDIR(statbuf.st_mode))
			{
				dir.push_back(direntp->d_name);
			}
		}
		else if(a && l && !R)  // -al or -la flag
		{
			print_l(direntp, statbuf);
		}
		else if(a && !l && R) 	// -aR or -Ra flag
		{
			printWithColor(statbuf, direntp);
			if(direntp->d_name[0] == '.')
			{
				if(direntp->d_name[1] != '.')
				{
					;
				}
				continue;
			}
			if(S_ISDIR(statbuf.st_mode))
			{
				dir.push_back(direntp->d_name);
			}
		}
		else if(!a && l && R) 	// -lR or -Rl flag
		{
			if(direntp->d_name[0] == '.')
			{
				continue;
			}
			print_l(direntp, statbuf);
		}
		else if(a && l && R) 	// -alR, -aRl, -laR, -lRa, -Ral, or -Rla flag
		{
			print_l(direntp, statbuf);
			if(direntp->d_name[0] == '.')
			{
				if(direntp->d_name[1] != '.')
				{
					;
				}
				continue;
			}
			if(S_ISDIR(statbuf.st_mode))
			{
				dir.push_back(direntp->d_name);
			}
		}
		else if(!a && !l && !R)	// no flags
		{
			if(direntp->d_name[0] == '.')
			{
				continue;
			}
			printWithColor(statbuf, direntp);
		}
		delete currentDirectory;
	}
	cout << endl << endl;
	const int size = dir.size();
	for(int i = 0; i < size; i++)
	{
		output(directory + "/" + dir.at(i), a, l, R);
	}

	if((closedir(dirp) == -1))
	{
		perror("Error: closedir failed");
		exit(1);
	}
	return ;
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
	for(int i = 0; i < inputs.size(); i++)
	{
		if(inputs.at(i).at(0) == '-')	// this means it's a flag
		{
			flags.push_back(inputs.at(i));
		}
		else if(inputs.at(i).at(0) == '/' || inputs.at(i) == "." || inputs.at(i) == "..")
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
	for(int i = 0; i < flags.size(); i++)
	{
		for(int j = 1; j < flags.at(i).size(); j++)
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
		}
	}

	if(directories.size() == 0)
	{
		directories.push_back(".");
		positions.push_back(failedCasePositions.size() + 1);
	}

	for(int i = 0 ; i < directories.size(); i++)
	{
		bool tF = false;
		if(failedCase.size() == 0)
		{
			output(directories.at(i), a, l, R);
			tF = true;
		}
		for(int j = 0; j < failedCase.size(); j++)
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
