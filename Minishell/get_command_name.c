#include "main.h"

char *get_command_name_by_pid(pid_t pid) // To print the commands name (for jobs)
{
    static char command[COMMAND_NAME_SIZE];             // buffer to store the command name
    char path[COMMAND_NAME_SIZE];                       // path to the proc file
    snprintf(path, sizeof(path), "/proc/%d/comm", pid); // create the path to the comm file

    FILE *file = fopen(path, "r"); // open the comm file
    if (file == NULL)
    {
        perror("fopen failed"); // Error handling
        return NULL;
    }

    if (fgets(command, sizeof(command), file) != NULL)
    { // Read the command name
        // Remove the newline character at the end if it exists
        size_t len = strlen(command);
        if (len > 0 && command[len - 1] == '\n')
        {
            command[len - 1] = '\0';
        }
    }
    else
    {
        perror("fgets failed"); // Error handling for reading from file
        fclose(file);
        return NULL;
    }

    fclose(file);
    return command; // Return the command name
}