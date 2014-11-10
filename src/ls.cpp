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
#include <algorithm>

#define GREEN "\033[1m\033[32m"
#define BLUE "\033[1m\033[34m"
#define END "\033[00m"
#define GRAY "\033[47m"
#define GRAYGREEN "\033[47m\033[1m\033[32m"
#define GRAYBLUE "\033[47m\033[1m\033[34m"

using namespace std;

void printTimeCreated(struct stat &buf)
{
	tm * tminfo = localtime(&(buf.st_mtime));
	char name[30];
	if(strftime(name, 30, "%b %d %H:%M", tminfo) == 0)
	{
		perror("Error: strftime failed");
	}
	cout << name << " ";
}

void printWithColor(struct stat statbuf, dirent * direntp)
{
	if(statbuf.st_mode & S_IFDIR) // directory = blue
	{
		cout << BLUE << direntp->d_name << END << " ";
	}
	else if(statbuf.st_mode & S_IXUSR) // executable = green
	{
		cout << GREEN << direntp->d_name << END << " ";
	}
	else if(direntp->d_name[0] == '.') // hidden files = gray
	{
		cout << GRAY << direntp->d_name << END << " ";
	}
	else if(direntp->d_name[0] == '.' && (statbuf.st_mode & S_IFDIR))
	{
		cout << GRAYBLUE << direntp->d_name << END << " ";
	}
	else if(direntp->d_name[0] == '.' && (statbuf.st_mode & S_IXUSR))
	{
		cout << GRAYGREEN << direntp->d_name << END << " ";
	}
	else
	{
		cout << direntp->d_name << " ";
	}	
}
void print_a(dirent* dir, struct stat & statbuf)
{
	printWithColor(statbuf, dir);
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
	
	if(!(pwd = getpwuid(statbuf.st_uid)))
	{
		perror("Error: getpwuid");
		exit(1);
	}
	cout << pwd->pw_name << " ";
	if(!(groupID = getgrgid(statbuf.st_gid)))
	{
		perror("Error: getgrgid");
		exit(1);
	}
	cout << groupID->gr_name << " ";
	
	cout << statbuf.st_size << " ";
	printTimeCreated(statbuf);
	printWithColor(statbuf, dir);
	cout << endl;
}
void output(vector <string> flags, vector <string> directories, bool a, bool l, bool R)
{
	struct stat statbuf;

	for(int i = 0; i < directories.size(); i++)
	{
		DIR *dirp;
		dirent *direntp;
		vector <dirent *> dir;
		if(!(dirp = opendir(directories.at(i).c_str())))
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

			string currentDir = directories.at(i);
			currentDir = currentDir + "/" + direntp->d_name;
			char * currentDirectory = new char[currentDir.size() + 1];
			strcpy(currentDirectory, currentDir.c_str());
			if((stat(currentDirectory, &statbuf)) == -1)
			{
				perror("Error: stat failed");
				exit(1);
			}
			dir.push_back(direntp);
			delete currentDirectory;
		}
		for(int i = 0; i < dir.size()-1; i++)
		{
			for(int j = 1; j < dir.size(); j++)
			{
				if(strcmp((dir.at(i)->d_name), (dir.at(j)->d_name)) < 0)
				{
					dirent * direnta = dir.at(i);
					dir.at(i) = dir.at(j);
					dir.at(j) = direnta;
				}
				
			}
		}
		for(int i = 0; i < dir.size(); i++)
		{
			if(!a && l && !R)
			{
				print_l(dir.at(i), statbuf);
			}
			else if(a && !l && !R)
			{
				print_a(dir.at(i), statbuf);
			}
			else if(!a && !l && R)
			{
				;
			}
			else if(a && l && !R)
			{
				print_l(dir.at(i), statbuf);
			}
			else if(a && !l && R)
			{
				;
			}
			else if(!a && l && R)
			{
				;
			}
			else if(a && l && R)
			{
				;
			}
			else if(!a && !l && !R)
			{
				printWithColor(statbuf, dir.at(i));
			}
		}
		cout << endl;
		if((closedir(dirp) == -1))
		{
			perror("Error: closedir failed");
			exit(1);
		}
		dir.clear();
	}
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

	for(int i = 0; i < inputs.size(); i++)
	{
		if(inputs.at(i).at(0) == '-')	// this means it's a flag
		{
			flags.push_back(inputs.at(i));
		}
		else if(inputs.at(i).at(0) == '/') 	// this means it's a directory
		{
			directories.push_back(inputs.at(i));
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
	if(directories.size() == 0) // this is for current directory
	{
		directories.push_back(".");
	}
	
	output(flags, directories, a, l, R);

	return 0;
}
