#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include<sys/wait.h>
#include<unistd.h>
#include "minishell.h"
extern char* external_cmds[154];
extern int status;
extern pid_t pid;
char *builtins[] = {
	"jobs","bg","fg","echo", "printf", "read", "cd", "pwd", "pushd", "popd",
	"dirs", "let", "eval", "set", "unset", "export", "declare",
	"typeset", "readonly", "getopts", "source", "exit", "exec",
	"shopt", "caller", "true", "type", "hash", "bind", "help", NULL
};

struct job jobs[20];
int job_count = 0;
pid_t foreground_pid = -1;

int copy_change(char *prompt, char *input_string)
{
	if(strncmp(input_string,"PS1=",4) !=0) 
		//validate the PS1 is proper space between equal or not
	{
		return 0;
	}
	strcpy(prompt,input_string+4);
	return 1;
}

void signal_handler(int sig_num)
{
	if(sig_num == SIGINT)
	{
		write(STDOUT_FILENO, "\n", 1);
	}
	else if(sig_num == SIGTSTP)
	{
		write(STDOUT_FILENO, "\n", 1);
	}
}

/* SIGCHLD handler */
void sigchld_handler(int sig)
{
	int child_status;
	pid_t child_pid;

	while((child_pid = waitpid(-1, &child_status, WNOHANG)) > 0)
	{

		if(child_pid == foreground_pid)
		{
			continue;
		}

		for(int i = 0; i < job_count; i++)
		{

			if(jobs[i].pid == child_pid)
			{

				if(WIFEXITED(child_status) ||
						WIFSIGNALED(child_status))
				{
					jobs[i].status = 0;
				}

				break;
			}
		}
	}
}
char* get_command(char *input_string)
{
	int count=0; 
	char array[100];// local array
	int i=0;
	while(input_string[i]!=' '&&input_string[i]!='\0')// compare upto end of string
	{
		array[i]=input_string[i];
		i++;
		count++;
	}
	array[i]='\0';
	char *str = malloc(sizeof(char)*count+1);// allocate memory dynamically
	strcpy(str,array);// copy from array to str
	return str;
}
int check_command_type(char *command)
{
	int i=0;
	// check the cmd is builtin or not
	while(builtins[i] != NULL)
	{ 
		if(strcmp(command,builtins[i])==0)// compare with builtin
		{
			return BUILTIN; 
		}
		if(strcmp(builtins[i],"echo")==0 && strncmp(command,"echo",4) ==0)
		{
			return BUILTIN;
		}
		i++;
	}
	i=0;
	while(external_cmds[i]!=NULL)
	{
		if(strcmp(command,external_cmds[i]) ==0)// compare with external commands
		{
			return EXTERNAL;
		}
		i++;
	}
	return NO_COMMAND;
}
void extract_external_commands(char ** external_commands)
{
	int fd = open("minishell_external_commands.txt",O_RDONLY);
	char ch;
	char array[50];
	int i=0; //index for local array
	int j=0; //index for external cmd array
	while(read(fd,&ch,1)>0)
	{
		if(ch == '\n')
		{
			array[i]= '\0'; 
			external_commands[j]=malloc(i+(1*sizeof(char)));// allocate memory for the external cmd with col size as the array size
			strcpy(external_commands[j],array); // copy the local array content into external_cmds.
			j++;
			i=0;
		}  
		else
		{
			array[i]=ch;
			i++;
		}
	}
}
void execute_internal_commands(char *input_string)
{
	char buffer[1000];
	if(strcmp(input_string,"pwd")==0)// check the input_string is pwd or not 
	{
		if(getcwd(buffer,sizeof(buffer)) != NULL)
		{
			printf("%s\n", buffer);
		}
		else
		{
			perror("getcwd");
		}
	}
	else if(strncmp(input_string,"cd",2)==0)// check the input_string is contains cd or not
	{
		char *path;
		if((path=(strchr(input_string,'/'))) != NULL)
		{
			int ret =chdir(path);//collect the return value and print success or failure
			if(ret==0)
			{
				printf("Successfully path readed\n");
			}
			else
			{
				printf("Failure to read the path\n");
			}
		}
	}
	else if(strcmp(input_string,"exit")==0) //check the input_string is exit or not 
	{
		exit(0);// to terminate all the process
	}
	else if(strncmp(input_string,"echo",4)==0)// check the input_string containing echo or not
	{
		echo(input_string);
	}

	else if(strcmp(input_string, "jobs") == 0)
	{
		for(int i = 0; i < job_count; i++)
		{
			if(jobs[i].status == 2)
			{
				printf("[%d]+ Stopped    %s\n",
						jobs[i].job_no,
						jobs[i].command);
			}
			else if(jobs[i].status == 1)
			{
				printf("[%d]+ Running    %s\n",
						jobs[i].job_no,
						jobs[i].command);
			}
		}
	}
	else if(strcmp(input_string, "bg") == 0)
	{
		if(job_count == 0)
		{
			printf("No jobs\n");
		}
		else
		{
			int i = job_count - 1;

			if(kill(jobs[i].pid, SIGCONT)==1)
			{
				perror("kill");
			}
			else
			{
				printf("SIGCONT sent successfully\n");
			}

			jobs[i].status = 1;

			printf("[%d]+ Running    %s &\n",
					jobs[i].job_no,
					jobs[i].command);
		}
	}
	else if(strcmp(input_string, "fg") == 0)
	{
		if(job_count == 0)
		{
			printf("No jobs\n");
		}
		else
		{
			int i = job_count - 1;

			printf("[%d]+ %s\n",
					jobs[i].job_no,
					jobs[i].command);

			// Continue the stopped/background process
			kill(jobs[i].pid, SIGCONT);

			// Wait for the process in foreground
			waitpid(jobs[i].pid, &status, WUNTRACED);

			if(WIFSTOPPED(status))
			{
				jobs[i].status = 2;
			}
			else if(WIFEXITED(status) || WIFSIGNALED(status))
			{
				// Job has finished, so remove it
				jobs[i].status = 0;
			}
		}
	}
	else
	{
		printf("Command not found\n");
	}
}
void echo (char *input_string)
{
	if(strcmp(input_string,"echo$$")==0)          // check the input_string is echo$$ or not
	{
		printf("%d\n",getpid());//print the minishell process id using getpid()
	}
	else if(strcmp(input_string,"echo$?")==0)          // check the input_string is echo$? or not
	{
		printf("%d\n",WEXITSTATUS(status));
	}
	else if(strcmp(input_string,"echo$SHELL")==0)          // check the input_string is echo$? or not
	{
		char *ret =getenv("SHELL");
		printf("%s\n",ret);
	}
	else
	{
		printf("Command not found\n"); 

	}

}
void execute_external_commands(char *input_string)
{
	int pipe_count = 0;

	/* Count number of pipes */
	for(int k = 0; input_string[k] != '\0'; k++)
	{
		if(input_string[k] == '|')
		{
			pipe_count++;
		}
	}

	/* Multiple pipes */
	if(pipe_count > 1)
	{
		execute_multiple_pipes(input_string);
		return;
	}

	/* No pipe - execute normal external command */
	if(pipe_count == 0)
	{
		char argv[20][20];
		char *args[20];
		char *token;
		int i = 0;

		/* Convert command into arguments */
		token = strtok(input_string, " ");

		while(token != NULL && i < 19)
		{
			strcpy(argv[i], token);
			i++;

			token = strtok(NULL, " ");
		}

		/* Convert character arrays to pointers */
		for(int k = 0; k < i; k++)
		{
			args[k] = argv[k];
		}

		args[i] = NULL;

		/* Execute command */
		execvp(args[0], args);

		/* execvp returns only if there is an error */
		perror("execvp");
		exit(1);
	}

	/* One pipe - execute two commands */

	char *cmd1;
	char *cmd2;

	/* Separate input into two commands */
	cmd1 = strtok(input_string, "|");

	/* Get second command */
	cmd2 = strtok(NULL, "|");

	char argv1[20][20];
	char argv2[20][20];

	char *token;
	int i = 0;
	int j = 0;

	/* Convert first command into arguments */
	token = strtok(cmd1, " ");

	while(token != NULL && i < 19)
	{
		strcpy(argv1[i], token);
		i++;

		token = strtok(NULL, " ");
	}

	/* Convert second command into arguments */
	token = strtok(cmd2, " ");

	while(token != NULL && j < 19)
	{
		strcpy(argv2[j], token);
		j++;

		token = strtok(NULL, " ");
	}

	char *args1[20];
	char *args2[20];

	/* Convert first 2D array to pointer array */
	for(int k = 0; k < i; k++)
	{
		args1[k] = argv1[k];
	}

	args1[i] = NULL;

	/* Convert second 2D array to pointer array */
	for(int k = 0; k < j; k++)
	{
		args2[k] = argv2[k];
	}

	args2[j] = NULL;

	int fd[2];

	/* Create pipe */
	if(pipe(fd) == -1)
	{
		perror("pipe");
		return;
	}

	/* Create first child */
	pid_t pid1 = fork();

	if(pid1 == -1)
	{
		perror("fork");
		return;
	}

	if(pid1 == 0)
	{
		/* First child */

		/* First command doesn't need read end */
		close(fd[0]);

		/* stdout -> pipe */
		dup2(fd[1], STDOUT_FILENO);

		/* Close original descriptor */
		close(fd[1]);

		/* Execute first command */
		execvp(args1[0], args1);

		perror("execvp");
		exit(1);
	}

	/* Create second child */
	pid_t pid2 = fork();

	if(pid2 == -1)
	{
		perror("fork");
		return;
	}

	if(pid2 == 0)
	{
		/* Second child */

		/* Second command doesn't need write end */
		close(fd[1]);

		/* stdin <- pipe */
		dup2(fd[0], STDIN_FILENO);

		/* Close original descriptor */
		close(fd[0]);

		/* Execute second command */
		execvp(args2[0], args2);

		perror("execvp");
		exit(1);
	}

	/* Parent doesn't need pipe */
	close(fd[0]);
	close(fd[1]);

	/* Wait for both pipe children */
	waitpid(pid1, &status, 0);
	waitpid(pid2, &status, 0);
}



void execute_multiple_pipes(char *input_string)
{
	char *commands[20];
	int count = 0;

	// Get the first command
	commands[count] = strtok(input_string, "|");

	// Get the remaining commands
	while(commands[count] != NULL && count < 19)
	{
		count++;
		commands[count] = strtok(NULL, "|");
	}


	char argv[20][20][20];
	char *args[20][20];

	int arg_count[20] = {0};

	// Convert every command into arguments
	for(int i = 0; i < count; i++)
	{
		int j = 0;

		char *token = strtok(commands[i], " ");

		while(token != NULL && j < 20)
		{
			strcpy(argv[i][j], token);
			// Remove double quotes from the argument
			if(argv[i][j][0] == '"' &&
					argv[i][j][strlen(argv[i][j]) - 1] == '"')
			{
				memmove(argv[i][j],
						argv[i][j] + 1,
						strlen(argv[i][j]) - 2);

				argv[i][j][strlen(argv[i][j]) - 2] = '\0';
			}
			j++;

			token = strtok(NULL, " ");
		}

		arg_count[i] = j;
	}

	// Convert 2D character arrays to argument pointers
	for(int i = 0; i < count; i++)
	{
		for(int j = 0; j < arg_count[i]; j++)
		{
			args[i][j] = argv[i][j];
		}

		args[i][arg_count[i]] = NULL;
	}
	// Print arguments for checking
	for(int i = 0; i < count; i++)
	{
		for(int j = 0; j < arg_count[i]; j++)
		{
			printf("args[%d][%d] = %s\n", i, j, args[i][j]);
		}
	}
	int fd[2][2];

	for(int i = 0; i < count - 1; i++)
	{
		if(pipe(fd[i]) == -1)
		{
			perror("pipe");
			return;
		}
		printf("Pipe %d created\n", i + 1);
	}

	pid_t pid[20];

	for(int i = 0; i < count; i++)
	{
		pid[i] = fork();

		if(pid[i] == -1)
		{
			perror("fork");
			return;
		}

		if(pid[i] == 0)
		{
			if(i==0)
			{
				// First command

				// Close read end of first pipe
				close(fd[0][0]);

				// Redirect stdout to first pipe
				dup2(fd[0][1], STDOUT_FILENO);

				// Close original write end
				close(fd[0][1]);

			}
			else if(i == count - 1)
			{
				// Last command

				// Close write end of last pipe
				close(fd[count - 2][1]);

				// Redirect stdin from last pipe
				dup2(fd[count - 2][0], STDIN_FILENO);

				// Close original read end
				close(fd[count - 2][0]);
			}
			else
			{
				// Middle command

				// stdin comes from previous pipe
				dup2(fd[i - 1][0], STDIN_FILENO);

				// stdout goes to next pipe
				dup2(fd[i][1], STDOUT_FILENO);
			}
			// Close all pipe descriptors
			for(int k = 0; k < count - 1; k++)
			{
				close(fd[k][0]);
				close(fd[k][1]);
			}
			// Execute the command
			fprintf(stderr, "Executing command %d: %s\n", i, args[i][0]);
			execvp(args[i][0], args[i]);

			perror("execvp");
			exit(1);
		}
	}
	// Parent closes all pipe descriptors
	for(int i = 0; i < count - 1; i++)
	{
		close(fd[i][0]);
		close(fd[i][1]);
	}


	// Parent waits for all children
	for(int i = 0; i < count; i++)
	{
		waitpid(pid[i], &status, 0);
	}
}
