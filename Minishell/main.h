#ifndef MAIN_H
#define MAIN_H
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <wait.h>
#include <ctype.h>
#include <limits.h>

#include <pwd.h>

#define SUCCESS 0
#define FAILURE 1

#define EXTERNAL 11
#define INTERNAL 22
#define PIPE 33

int execute_external_command(char *input);
void execute_internal_command(char *input);
void get_command(char *input, char *command);
int check_command_type(char *input);
int IsSpecialVariables(char *input);
void fg();
void bg();
void jobs_list();
void check_background_jobs();
void execute_internal_command(char *input);
void execute_piped_commands(char *input);
char *get_command_name_by_pid(pid_t pid);

#define MAX_JOBS 100
#define MAX_COMMANDS 10
#define COMMAND_NAME_SIZE 256

typedef struct job
{
    pid_t pid;      // Process ID
    int is_running; // Job status: 1 = running, 0 = stopped
    char cmd[256];  // Command string
} job_t;

job_t jobs[MAX_JOBS]; // Jobs table
extern int job_count; // Declaration only

#endif