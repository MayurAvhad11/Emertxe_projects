#include "main.h"

int update_database(file_node **head, main_ **arr, char *filename)
{
    if ((*head) == NULL)
    {
        printf("NO database is there to be updated\n");
        return 1;
    }
    if (strcmp(strstr(filename, "."), ".txt") == 0)
    {
        // 1.its txt file
        printf("%s is a .txt file\n", filename);
        FILE *fptr = fopen(filename, "r");

        if (fptr == NULL) // 2.is it exist or not?
        {
            printf("%s not exists\n", filename);
        }
        else
        {
            // 3.is it empty ?
            fseek(fptr, 0, SEEK_END);
            if (ftell(fptr) == 0)
            {
                printf("%s is empty\n", filename);
                fclose(fptr);
                return 1;
            }
            else
            {
                // file not empty
                // 4. is it already in database ?

                file_node *temp = *head;
                file_node *prev_temp = NULL;
                while (temp)
                {
                    if (strcmp(temp->filename, filename) == 0)
                    {
                        printf("File is Already in database\n");
                        return 1;
                    }
                    prev_temp = temp;
                    temp = temp->link;
                }
                // new node for new file
                file_node *fnew = malloc(sizeof(file_node));
                prev_temp->link = fnew;
                strcpy(fnew->filename, filename);
                fnew->link = NULL;

                fseek(fptr, 0, SEEK_SET);

                create_database(arr, *head);
            }
        }
    }
}