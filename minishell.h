#ifndef MAIN_H
#define MAIN_H

#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio_ext.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>


#define BUILTIN		1
#define EXTERNAL	2
#define NO_COMMAND      3

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

struct job
{
    int job_no;
    pid_t pid;
    char command[100];
    int status;
};

extern struct job jobs[20];
extern int job_count;
extern pid_t foreground_pid;
extern pid_t foreground_pid;

void scan_input(char *prompt, char *input_string);
char *get_command(char *input_string);
int copy_change(char *prompt, char *input_string);
int check_command_type(char *command);
void execute_internal_commands(char *input_string);
void echo (char *input_string);
void execute_external_commands(char *input_string);
void signal_handler(int sig_num);
void extract_external_commands(char **external_commands);
void execute_multiple_pipes(char *input_string);
void sigchld_handler(int sig);

/*List o builtin commands*/
extern char *builtins[];
#endif
