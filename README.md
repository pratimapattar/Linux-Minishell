# Linux Mini Shell

## Project Overview

Linux Mini Shell is a command-line shell application developed in C using Linux system programming concepts.
The project demonstrates how a basic shell works by accepting user commands, identifying built-in and external commands, 
creating child processes, executing commands, handling signals, and implementing command piping.
It provides a custom shell prompt and supports selected internal commands and external Linux commands.

## Objectives

- Understand the working of a basic Linux shell.
- Learn process creation and management using system calls.
- Execute external Linux commands.
- Implement built-in command handling.
- Understand signal handling.
- Implement inter-process communication using pipes.

## Features

- **Custom Shell Prompt:** Displays a user-defined shell prompt.
- **Prompt Modification:** Supports changing the prompt using `PS1`.
- **Built-in Commands:** Supports commands such as `cd`, `pwd`, `echo`, `exit`, `jobs`, `bg`, and `fg`.
- **External Command Execution:** Executes external Linux commands using `execvp()`.
- **Process Creation:** Creates child processes using `fork()`.
- **Signal Handling:** Handles `SIGINT`, `SIGTSTP`, and `SIGCHLD`.
- **Job Management:** Maintains information about stopped and running jobs.
- **Single Pipe Execution:** Connects two commands using a pipe.
- **Multiple Pipe Execution:** Implements execution of commands connected through multiple pipes.

## Technologies Used

- **Programming Language:** C
- **Operating System:** Linux
- **Compiler:** GCC
- **System Calls:** `fork()`, `execvp()`, `waitpid()`, `pipe()`, `dup2()`, `open()`, `read()`, `write()`, `kill()`
- **Signal Handling:** `sigaction()`, `signal()`

## C and Linux Concepts Used

- Process creation and management
- Parent and child processes
- Process execution using `execvp()`
- Inter-process communication (IPC)
- Pipes and file descriptors
- Input/output redirection using `dup2()`
- Signal handling
- Foreground and background job management
- File handling
- Command-line input processing
- Structures and pointers
- Header files and modular programming

## Project Structure

```text
Linux-Minishell/
│
├── minishell_main.c
├── minishell.c
├── minishell.h
├── minishell_external_commands.txt
├── README.md
└── .gitignore
```

## Compilation

Open the Linux terminal inside the project directory.

Compile the source files using GCC:

```bash
gcc minishell_main.c minishell.c -o minishell
```

## Execution

Run the application using:

```bash
./minishell
```

The custom shell prompt will appear, allowing the user to enter supported commands.

## Example Commands

### Built-in Commands

```bash
pwd
cd /home
echo$$
echo$?
echo$SHELL
jobs
bg
fg
exit
```

### External Commands

```bash
ls
date
cat
ps
whoami
```

### Pipe Operations

Single pipe:

```bash
ls | wc
```

Multiple pipes:

```bash
ls | grep .c | wc
```

*Command support depends on the implementation and the external command list provided with the project.*

## System Calls Used

| System Call | Purpose |
|---|---|
| `fork()` | Creates a child process |
| `execvp()` | Executes an external command |
| `waitpid()` | Waits for child process termination or state changes |
| `pipe()` | Creates a communication channel between processes |
| `dup2()` | Redirects standard input/output |
| `kill()` | Sends signals to processes |
| `open()` | Opens a file |
| `read()` | Reads data from a file |
| `write()` | Writes data to a file |

## Learning Outcomes

Through this project, I gained practical experience in:

- Understanding Linux process management.
- Working with parent and child processes.
- Executing external commands using system calls.
- Implementing inter-process communication using pipes.
- Understanding file descriptors and I/O redirection.
- Handling signals in Linux.
- Managing foreground and background processes.
- Developing a modular C application in a Linux environment.

## Author
**Pratima C Pattar**
