
#include "main.h"
#include <sys/wait.h>

extern char PS1[];
char prev_dir[PATH_MAX] = ""; // Store the previous directory

extern int job_count; // Declaration of job_count
extern int pending_jobs;

void check_background_jobs()
{
    int status;
    pid_t pid;

    for (int i = 0; i < job_count; i++)
    {
        if (jobs[i].is_running)
        {
            pid = waitpid(jobs[i].pid, &status, WNOHANG); // Non-blocking wait
            if (pid == 0)
            {
                // The process is still running
                continue;
            }
            else if (pid > 0)
            {
                // The process has changed state
                if (WIFEXITED(status) || WIFSIGNALED(status))
                {
                    printf("Job [%d] with PID %d has finished\n", i + 1, jobs[i].pid);
                    jobs[i].is_running = 0; // Mark as finished
                    job_count--;
                }
            }
            else
            {
                perror("waitpid failed");
            }
        }
    }
}

void jobs_list()
{
    if (job_count == 0)
    {
        printf("No background jobs\n");
        return;
    }

    // printf("Jobs list:\n");
    for (int i = 0; i < job_count; i++)
    {
        printf("[%d] %d %s %s\n", i + 1, jobs[i].pid,
               jobs[i].is_running ? "Running" : "Stopped",
               jobs[i].cmd != NULL && strlen(jobs[i].cmd) > 0 ? jobs[i].cmd : "(command not available)");
    }
}
void bg() // bg
{
    pid_t pid = jobs[job_count - 1].pid;
    printf("Resuming job [%d] %d in background\n", job_count, pid);
    jobs[job_count - 1].is_running = 1;

    // Send SIGCONT to the job to resume it
    kill(pid, SIGCONT);
}

void fg() //
{
    pid_t pid = jobs[job_count - 1].pid;
    int status;
    // Bring the background job to the foreground by sending SIGCONT
    printf("Bringing job [%d] with PID %d to the foreground\n", job_count, pid);
    jobs[job_count - 1].is_running = 1; // Mark as running

    // Send SIGCONT to the job to resume it
    if (kill(pid, SIGCONT) == -1)
    {
        perror("Error sending SIGCONT");
        return;
    }

    // Wait for the job to finish in the foreground
    if (waitpid(pid, &status, WUNTRACED) == -1)
    {
        perror("Error waiting for job");
    }

    // Check if the process was stopped or exited
    if (WIFSTOPPED(status))
    {
        printf("Job [%d] with PID %d was stopped\n", job_count, pid);
    }
    else if (WIFEXITED(status))
    {
        printf("Job [%d] with PID %d exited with status %d\n", job_count, pid, WEXITSTATUS(status));
        jobs[job_count - 1].is_running = 0; // Mark as finished
        job_count--;
    }
    else if (WIFSIGNALED(status))
    {
        printf("Job [%d] with PID %d was terminated by signal %d\n", job_count, pid, WTERMSIG(status));
        jobs[job_count - 1].is_running = 0; // Mark as finished
        job_count--;
    }
}

void execute_internal_command(char *input)
{
    char cmd[256];
    char *home_dir = getenv("HOME");
    char *curr_dir = NULL;

    if (!strncmp(input, "cd", 2))
    {
        if (strlen(input) == 2) // no arguments: just "cd" -> go to home directory
        {
            chdir(home_dir);
        }
        else
        {
            strcpy(cmd, input + 3); // Extract command after "cd "

            // Handle special cases
            if (!strcmp(cmd, "~")) // cd ~
            {
                chdir(home_dir);
            }
            else if (!strcmp(cmd, ".")) // cd . (current directory)
            {
                // Do nothing
            }
            else if (!strcmp(cmd, "..")) // cd .. (parent directory)
            {
                chdir("..");
            }
            else if (!strcmp(cmd, "-")) // cd - (previous directory)
            {
                if (strlen(prev_dir) == 0)
                {
                    printf("No previous directory\n");
                }
                else
                {
                    printf("%s\n", prev_dir);
                    chdir(prev_dir);
                }
            }
            else if (cmd[0] == '~') // cd ~username (go to another user's home directory)
            {
                struct passwd *pw = getpwnam(cmd + 1); // Get home directory for user
                if (pw != NULL)
                {
                    chdir(pw->pw_dir);
                }
                else
                {
                    printf("User not found\n");
                }
            }
            else if (chdir(cmd) == -1) // Handle invalid directory
            {
                perror("cd failed");
                return;
            }
        }

        // Get current directory and update PS1
        curr_dir = get_current_dir_name();
        if (curr_dir == NULL)
        {
            perror("get_current_dir_name failed");
            return;
        }

        // Store the previous directory
        strcpy(prev_dir, curr_dir);

        // Update PS1 dynamically based on current directory length
        snprintf(PS1, strlen(curr_dir) + 13, "minishell:%s$ ", curr_dir);

        free(curr_dir); // Free dynamically allocated memory for current directory
    }

    else if (!strncmp(input, "pwd", 3)) // pwd cmd
    {
        printf("%s\n", get_current_dir_name());
    }
    else if (strncmp(input, "exit", 4) == 0) // exit cmd
    {
        int exit_code = 0;                  // Default exit code
        char *arg = strtok(input + 5, " "); // Get argument after "exit"

        if (arg != NULL)
        {
            // Convert argument to an integer (check for errors)
            exit_code = atoi(arg);
        }

        // Exit the shell
        exit(exit_code);
    }
    else if (!strncmp(input, "fg", 2)) // fg command
    {
        fg();
    }
    else if (!strncmp(input, "bg", 2)) // bg command
    {

        bg();
    }
    else if (!strncmp(input, "jobs", 4)) // jobs command
    {
        jobs_list();
    }
}