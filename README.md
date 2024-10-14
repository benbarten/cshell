# CShell
CShell is a simple command-line shell implemented in C. It supports basic built-in POSIX commands and can execute external commands. 

_Note: This is just an educational project of mine to teach myself come basic C. Do with it whatever you like:)_

## Features
Built-in commands:
* `pwd`: Print the current working directory.
* `cd <directory>`: Change the current directory.
* `echo <message>`: Print a message to the terminal.
* `exit`: Exit the shell.
* Execution of external commands in your `$PATH`.
* Environment variable expansion.
* Get/set/list environment variables.

### Getting Started

#### Prerequisites
* GCC or any C compiler
* Make (optional)

#### Building
To build the project, run:

```sh
gcc -o cshell main.c builtin.c external.c parse.c envvars.c
```

Alternatively, if you have make installed, you can use the provided Makefile:

```sh
make
```

#### Running
To start the shell, run:

```sh
./cshell
```

