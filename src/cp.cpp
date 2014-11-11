	#include <iostream>
	#include <unistd.h>
	#include <stdio.h>
	#include <sys/syscall.h>
	#include "Timer.h"
	#include <fstream>
	#include <fcntl.h>
	#include <string.h>
	#include <sys/types.h>
	#include <sys/stat.h>
	using namespace std;
	
	int main(int argc, char **argv)
	{
		char OPT1[2] = "1";
		char OPT2[2] = "2";
		char OPT3[2] = "3";
		Timer x;
		double num;
		x.start();
	
		if(argc == 4 && strcmp(argv[3], OPT1) == 0)
		{ 
			ifstream inFS;
			ofstream outFS;
			char input;
			
			if(access(argv[2], F_OK) == 0)
			{
				cout << "File already exists" << endl;
				return 1;
			}
			inFS.open(argv[1]);
			outFS.open(argv[2]);		
			while(!inFS.eof())
			{
				inFS.get(input);
				outFS.put(input);
			}
			inFS.close();
			outFS.close();
		}
		
		else if(argc == 4 && strcmp(argv[3], OPT2) == 0) 
		{
			char array[1];
			int readError;
			memset(array, 0, sizeof(array));
			if(access(argv[2], F_OK) == 0)
			{
				cout << "File already exists" << endl;
				return 1;
			}
			int fd = open(argv[1], O_RDONLY);
			if(fd == -1)
			{
				perror("open");
				return 1;
			}
			int fd2 = open(argv[2], O_WRONLY | O_CREAT, 00700);
			if(fd2 == -1)
			{
				perror("open 2");
				return 1;
			}
			
			while((readError= read(fd, array, sizeof(array))) != 0)
			{
				if(readError == -1)
				{
					perror("read");
					return 1;
				}
				if(write(fd2, array, readError) == -1)
				{
					perror("write");
					return 1;
				}
			}
			close(fd);
			close(fd2);
		}
	
		else if ((argc == 4 && strcmp(argv[3], OPT3) == 0) || argc == 3)
		{
			char array[BUFSIZ];
			int readError;
			memset(array, 0, sizeof(array));
			if(access(argv[2], F_OK) == 0)
			{
				cout << "File already exists" << endl;
				return 1;
			}
			int fd = open(argv[1], O_RDONLY);
			if(fd == -1)
			{
				perror("open");
				return 1;
			}
			int fd2 = open(argv[2], O_WRONLY | O_CREAT, 00700);
			if(fd2 == -1)
			{
				perror("open 2");
				return 1;
			}
			
			while((readError= read(fd, array, sizeof(array))) != 0)
			{
				if(readError == -1)
				{
					perror("read");
					return 1;
				}
				if(write(fd2, array, readError) == -1)
				{
					perror("write");
					return 1;
				}
			}
			close(fd);
			close(fd2);
		}
	
		else
		{
			cout << "Error: not enough arguments" << endl;
			return 1;
		}
	
		x.elapsedUserTime(num);
		cout << num << endl;
		return 0;
	}