#include <stdio.h>
#include <string.h>
#include "minishell.h"
char *external_cmds[154];
int status;
pid_t pid;
int main()
{
    struct sigaction act;
    struct sigaction chld_act;

    /* SIGINT and SIGTSTP handler */
    sigemptyset(&act.sa_mask);
    act.sa_handler = signal_handler;
    act.sa_flags = SA_RESTART;

    sigaction(SIGINT, &act, NULL);
    sigaction(SIGTSTP, &act, NULL);


    /* SIGCHLD handler */
    sigemptyset(&chld_act.sa_mask);
    chld_act.sa_handler = sigchld_handler;
    chld_act.sa_flags = SA_RESTART;

    sigaction(SIGCHLD, &chld_act, NULL);


    extract_external_commands(external_cmds);

    system("clear");

    char prompt[50] = "minishell$";
    char input_string[100];

    scan_input(prompt, input_string);

    return 0;
}


void scan_input(char * prompt,char *input_string)
{
	while(1)
	{
		printf("%s",prompt);//print the prompt using printf
		fflush(stdout);

		if(fgets(input_string, 100, stdin) == NULL)
		{
			if(errno == EINTR)
			{
				clearerr(stdin);
				printf("%s", prompt);
				fflush(stdout);
				continue;
			}

			break;
		}

		input_string[strcspn(input_string, "\n")] = '\0';
		if(strstr(input_string,"PS1")!= NULL)
		{
			copy_change(prompt,input_string);
			continue;
		}
		else
		{
			char *cmd = get_command(input_string);
			int ret = check_command_type(cmd);
			if(ret == BUILTIN)
			{
				printf("BUILTIN\n");
				execute_internal_commands(input_string);
			}
			else if(ret == EXTERNAL)
			{
				printf("EXTERNAL\n");
				pid =fork();
				if(pid == 0) 
				{
					// child should receive normal ctrl+C / ctrl + Z behaviour
					signal(SIGINT, SIG_DFL);
					signal(SIGTSTP, SIG_DFL);
					execute_external_commands(input_string);
					exit(0);
				}
				else if(pid>0)
				{
					foreground_pid = pid;
					waitpid(pid,&status,WUNTRACED);
					if(WIFSTOPPED(status))
					{
						jobs[job_count].job_no = job_count + 1;
						jobs[job_count].pid = pid;

						strcpy(jobs[job_count].command, input_string);

						jobs[job_count].status = 2;                                           
						printf("\n[%d]+ Stopped    %s\n", jobs[job_count].job_no,
								jobs[job_count].command);
						job_count++;
					}
					foreground_pid = -1;
				}                       
			}
			else
			{
				printf("COMMAND NOT FOUND\n");
			}
		}
	}
}
