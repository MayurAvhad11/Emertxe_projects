/*
Name:Mayur Anil Avhad
Date:11/10/2024
Description:
Minishell -
       -Implement a minimalistic shell, mini-shell(msh) as part of the Linux Internal module.
Sample input:
minishell$ date

Sample Output: Fri Oct 11 12:40:55 IST 2024

*/

#include "main.h"
#include <unistd.h>
#include <signal.h>
#include <stdbool.h>

bool stopped = false;
int exit_status; // for exit status of pre cmd executed
int job_count = 0;
int pending_jobs = 0;

char *internal_cmd[] = {"cd", "exit", "pwd", "fg", "bg", "jobs", NULL};
char PS1[100] = "minishell$ ";
int fg_job_pid = -1;  // PID of the foreground job
int fg_job_pgid = -1; // PGID of the foreground job

// for handle the signals
void sigint_handler(int sig)
{
    // Send SIGINT to the foreground job
    if (fg_job_pid > 0)
    {
        killpg(fg_job_pgid, SIGINT); // Send signal to the process group
    }
    printf("\n");
}

void sigtstp_handler(int sig)
{
    if (fg_job_pid > 0) // check signal is from forground process
    {
        // Check if the job is already in the jobs array
        int job_found = 0;
        for (int i = 0; i < job_count; i++)
        {
            if (jobs[i].pid == fg_job_pid)
            {
                jobs[i].is_running = 0; // Mark as stopped
                job_found = 1;
                break;
            }
        }

        // If the job was not found, add it to the list of jobs
        if (!job_found && job_count < MAX_JOBS) // Make sure to check against maximum jobs allowed
        {
            jobs[job_count].pid = fg_job_pid;
            char *command_name = get_command_name_by_pid(jobs[job_count].pid);              // geting the cmd name by pid
            jobs[job_count].is_running = 0;                                                 // mark as stopped
            snprintf(jobs[job_count].cmd, sizeof(jobs[job_count].cmd), "%s", command_name); // store the cmd name

            job_count++; // Increment job count
        }

        printf("\nJob stopped (PID: %d)\n", fg_job_pid);
        pending_jobs = 1; // Set flag for pending jobs
    }
    printf("\n");
}

int check_command_type(char *input) // ex. cd directory
{
    if (strchr(input, '|') != NULL)
    {
        return PIPE; // Return PIPE if found
    }
    // extract the cmd from input
    char cmd[strlen(input) + 1];
    get_command(input, cmd);

    for (int i = 0; internal_cmd[i] != NULL; i++)
    {
        if (!strcmp(cmd, internal_cmd[i]))
        {
            return INTERNAL;
        }
    }
    return EXTERNAL;
}
void get_command(char *input, char *command)
{
    int i = 0;
    for (; input[i] != ' ' && input[i] != '\0'; i++)
    {
        command[i] = input[i];
    }
    command[i] = '\0';
}
int IsSpecialVariables(char *input) // input: echo $$
{

    // check if input has command 'echo'
    // 1.extract only command from the input
    char command[strlen(input) + 1];
    get_command(input, command); // extracting cmd

    if (!strcmp(command, "echo"))
    {
        strcpy(command, input + 5); // echo (4+space)

        if (!strcmp(command, "$$")) //$$->gives pid of minishell
        {
            printf("%d\n", getpid());
        }
        else if (!strcmp(command, "$?")) //$?->gives exit status of prev command
        {
            printf("%d\n", exit_status);
        }
        else if (!strcmp(command, "$SHELL")) //$SHELL->gives path of the minishell
        {
            printf("%s/minishell\n", get_current_dir_name());
        }
        else
        { // print as it is
            printf("%s\n", command);
        }
        return SUCCESS;
    }
    return FAILURE;
}

int main()
{
    // display minishell$ and wait for user input.

    char input[50];

    struct sigaction sa_int, sa_tstp;
    sa_int.sa_handler = sigint_handler;
    sigemptyset(&sa_int.sa_mask);
    sa_int.sa_flags = 0;
    sigaction(SIGINT, &sa_int, NULL);

    sa_tstp.sa_handler = sigtstp_handler;
    sigemptyset(&sa_tstp.sa_mask);
    sa_tstp.sa_flags = 0;
    sigaction(SIGTSTP, &sa_tstp, NULL);

    while (1)
    {
        printf("%s", PS1);
        fgets(input, 50, stdin); // input = PS1=NEW_PROMT

        // to remove the '\n'
        input[strlen(input) - 1] = '\0';

        if (!strncmp(input, "PS1=", 4)) // checking if starting 4 char in input is PS1=
        {
            strcpy(PS1, input + 4); // copy the char's after PS1=
        }
        else if (IsSpecialVariables(input) == SUCCESS) // already executed echo
        {
            continue;
        }
        else if (check_command_type(input) == INTERNAL)
        {
            execute_internal_command(input); // execute internal cmd
        }
        else if (check_command_type(input) == EXTERNAL)
        {
            execute_external_command(input); // execute external command
        }
        else if (check_command_type(input) == PIPE) // check if input has pipe
        {
            execute_piped_commands(input); // execute piped cmd's
        }
        check_background_jobs(); // check for any background jobs periodically
    }
}