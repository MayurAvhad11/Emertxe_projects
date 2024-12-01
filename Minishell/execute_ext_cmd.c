#include "main.h"

extern int job_count;   // Declaration of job_count
extern int fg_job_pid;  // PID of the foreground job
extern int fg_job_pgid; // PGID of the foreground job

int execute_external_command(char *input)
{
    char *cmd[5];
    char *token;
    int ret = fork(); // creating the child process
    int status;
    int background = 0;

    // Trim trailing whitespace
    int len = strlen(input);
    while (len > 0 && isspace(input[len - 1]))
    {
        len--;
    }
    input[len] = '\0'; // Null-terminate after trimming

    // Check if the last character is '&' (background job)
    if (input[len - 1] == '&')
    {
        background = 1;        // run cmd in background
        input[len - 1] = '\0'; // Remove '&' from the input
        len--;                 // Adjust the length after removing '&'
    }

    token = strtok(input, " ");
    int i = 0;

    // Populate cmd array with command and arguments
    while (token != NULL && i < 5)
    {
        cmd[i] = token;
        token = strtok(NULL, " ");
        i++;
    }
    cmd[i] = NULL; // Last element must be NULL for execvp

    if (ret == 0)
    { // child process
        if (execvp(cmd[0], cmd) == -1)
        {
            perror("execvp failed");
            exit(errno); // Exit with error code
        }
    }
    else if (ret > 0)
    {                    // parent process
        if (!background) // if process is foreground
        {
            fg_job_pid = ret;                     // Set the foreground job PID
            fg_job_pgid = getpgid(ret);           // Get the PGID of the child
            tcsetpgrp(STDIN_FILENO, fg_job_pgid); // Set terminal control to the foreground job
            waitpid(ret, &status, WUNTRACED);     // Wait for the foreground process
            // Restore terminal control back to the shell after the job completes
            tcsetpgrp(STDIN_FILENO, getpid());
            fg_job_pid = -1;  // Reset fg_job_pid after completion
            fg_job_pgid = -1; // Reset fg_job_pgid after completion
        }
        else
        { // runnig in background
            printf("[%d] %d running in background\n", job_count + 1, ret);
            jobs[job_count].pid = ret;
            jobs[job_count].is_running = 1;
            strcpy(jobs[job_count].cmd, cmd[0]);
            job_count++;
        }

        // check_background jobs periodically
        check_background_jobs();
    }

    else
    {
        perror("fork failed");
        return -1;
    }
    return 0;
}
