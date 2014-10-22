RSHELL
=======

Licensing Information: READ LICENSE
---
Project source can be downloaded from https://github.com/rluk001/rshell.git
----

Author & Contributor List
-----------
Mike Izbicki

Ryan Luk

All other known bugs and fixes can be set to rluk001@ucr.edu

Reported bugs/fixes will be submitted to correction.

File List
---------
```
.:

Makefile

LICENSE

README.md

./src

./tests
```
```
/src:

rshell.cpp
```
```
./tests:

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

Bugs/Limitations
---------
```

*When running the shell, it is unable to take in more than one command.
*Does not support multiple types of connectors on one line.
*& and | are not intrepreted by the program and cause a fail.


