#include "main.h"

void execute_piped_commands(char *input)
{
    int pipe_value = 0;
    int cmd_len = 0;

    // Count the number of pipes
    for (int i = 0; input[i] != '\0'; i++)
    {
        if (input[i] == '|')
        {
            pipe_value++;
        }
    }
    // printf("The number of pipes %d\n", pipe_value);

    // Store positions of the pipes
    int pipe_pos[pipe_value], j = 0;
    for (int i = 0; input[i] != '\0'; i++)
    {
        if (input[i] == '|')
        {
            pipe_pos[j] = i;
            j++;
        }
    }

    char ***commands = (char ***)malloc((pipe_value + 1) * sizeof(char **));
    if (commands == NULL)
    {
        printf("Memory allocation failed!\n");
        return;
    }

    // Split commands based on pipe positions
    int start = 0;
    for (int i = 0; i <= pipe_value; i++)
    {
        int end = (i < pipe_value) ? pipe_pos[i] : strlen(input);
        cmd_len = end - start;

        // Allocate space for each command
        char *cmd = (char *)malloc((cmd_len + 1) * sizeof(char));
        strncpy(cmd, input + start, cmd_len);
        cmd[cmd_len] = '\0';

        // Allocate for the command's arguments (split tokens)
        char **args = (char **)malloc(10 * sizeof(char *)); // max 10 tokens
        int k = 0;
        char *token = strtok(cmd, " ");
        while (token != NULL)
        {
            args[k] = strdup(token); // allocate and copy token to avoid freeing issue
            token = strtok(NULL, " ");
            k++;
        }
        args[k] = NULL; // Null-terminate the command array

        commands[i] = args; // Assign the command arguments to commands array
                            // printf("Command %d: %s\n", i, commands[i][0]); // For debugging

        start = end + 1; // Move start to the next command
        free(cmd);       // Free the temporary command string after use
    }

    // Variables for piping and forking
    int pipefd[2];
    int prev_pipe = -1;

    // Create child processes to handle the commands
    for (int i = 0; i <= pipe_value; i++)
    {
        if (i < pipe_value)
        {
            if (pipe(pipefd) == -1)
            {
                perror("pipe");
                exit(EXIT_FAILURE);
            }
        }

        int pid = fork();
        if (pid == -1)
        {
            perror("fork");
            exit(EXIT_FAILURE);
        }

        if (pid == 0) // Child process
        {
            if (prev_pipe != -1)
            {
                dup2(prev_pipe, STDIN_FILENO);
                close(prev_pipe);
            }

            if (i < pipe_value)
            {
                close(pipefd[0]);
                dup2(pipefd[1], STDOUT_FILENO);
                close(pipefd[1]);
            }

            // printf("Executing command: %s\n", commands[i][0]);
            execvp(commands[i][0], commands[i]); // execute the cmd
            perror("execvp failed");
            exit(EXIT_FAILURE);
        }
        else // Parent process
        {
            if (prev_pipe != -1)
            {
                close(prev_pipe);
            }

            if (i < pipe_value)
            {
                close(pipefd[1]);
                prev_pipe = pipefd[0];
            }
        }
    }

    // wait for all children to finish
    for (int i = 0; i <= pipe_value; i++)
    {
        wait(NULL);
    }

    // free allocated memory
    for (int i = 0; i <= pipe_value; i++)
    {
        char **args = commands[i];
        for (int j = 0; args[j] != NULL; j++)
        {
            free(args[j]); // free each token
        }
        free(args); // free the argument array
    }
    free(commands); // free the commands array
}
