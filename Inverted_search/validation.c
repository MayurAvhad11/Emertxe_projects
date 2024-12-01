#include "main.h"

int validation(file_node **head, char *filename[], int argc)
{
    for (int i = 1; i < argc; i++)
    {
        if (filename[i] != NULL) // check exist or not
        {
            if (strcmp(strstr(filename[i], "."), ".txt") == 0)
            {
                printf("%s is a .txt file\n", filename[i]);
                FILE *fptr = fopen(filename[i], "r");
                if (fptr == NULL)
                {
                    printf("file not exist\n");
                }
                else
                {
                    printf("file is present\n");
                    // file is opend
                    // checking the file is empty or not
                    fseek(fptr, 0, SEEK_END); // moving the fptr to end of the file
                    if (ftell(fptr) == 0)     // means its empty
                    {
                        printf("File is emtpy\n");
                        fclose(fptr); // Close the file after checking
                        continue;
                    }
                    else
                    {
                        printf("file is not empty\n");
                        // check dubplicate or not
                        // first file will not be a dubplicate
                        if (*head == NULL)
                        {
                            // update the file name & add it to list
                            file_node *new = malloc(sizeof(file_node));
                            if (new == NULL)
                            {
                                printf("Failed to create node for filename\n");
                                // return FAILURE;
                            }
                            (*head) = new;
                            new->link = NULL;
                            strcpy(new->filename, filename[i]);
                        }
                        else // check for duplicated files
                        {
                            file_node *temp = *head;
                            while (temp->link != NULL)
                            {
                                // compare filename (ignore duplicated)
                                if (strcmp(temp->filename, filename[i]) == 0)
                                {
                                    printf("File %s is already present\n", filename[i]);
                                    return 0;
                                }
                                temp = temp->link;
                            }
                            // if its not duplicated add file name to linked list
                            file_node *new = malloc(sizeof(file_node));
                            new->link = NULL;
                            strcpy(new->filename, filename[i]);
                            temp->link = new;
                        }
                    }
                }
            }
            else
            {
                printf("%s is not a .txt file\n", filename[i]);
            }
        }
    }
}