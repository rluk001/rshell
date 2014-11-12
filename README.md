RSHELL
=======

---

Author & Contributor List
-----------
Ryan Luk

All other known bugs and fixes can be set to rluk001@ucr.edu

Reported bugs/fixes will be submitted to correction.

How to run program and install
----------

1. Open a terminal and clone the repository. *git clone http://github.com/rluk001/rshell.git*

2. Then cd into the project *cd rshell*

3. Create executables with *make*

4. Execute rshell *bin/rshell*

Using RShell	
---------
```
Multiple commands can be used together on the same line using ; , &&  or || as connectors.

Connecting multiple commands with ; will execute every command on their own regardless of the prior command's success or failure.

Connecting multiple commands with && will execute every command in order until one fails.

Does NOT support multiple types of connectors on one line.

Extra connectors in front of and after commands do nothing.

Anything after a # will be ignored.

To exit RShell, use the command exit.

```

Bugs/Limitations of rshell
---------
```

1. When running the shell, it is unable to handle the && connector and the || connector together and print it out successfully.

2. Requires a semi-colon to run the program with the && and || on the same line.

3. Running multiple & and | can cause the program to crash.

4. Doesn't check if there is two &'s or |'s when it is used

```

Bugs/Limitations of ls
---------

1. The recursive case might cause errors when used on really big cases from symbolic link issues.

2. There are extra newlines after ls output and it depends on what flags are used.

3. Files are not sorted the same way as the ls used in bash.

4. It doesn't print out nicely in columns. It is printed out two spaces at once for "-a".

5. The total blocks space on -l is not added correctly. It is slightly off the correct amount.
```
