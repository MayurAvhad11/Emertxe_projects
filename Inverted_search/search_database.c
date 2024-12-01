#include "main.h"

int search_database(main_ **arr, char *search_word)
{
    int index = search_word[0] % 97;
    if (arr[index] != NULL)
    {
        main_ *temp = arr[index]; // main node
        while (temp)
        {
            if (strcmp(temp->word, search_word) == 0)
            {
                printf("word found\n");
                printf("word : %s,filecount : %d", temp->word, temp->filecount);
                sub *subtemp = temp->sublink;
                while (subtemp)
                {
                    printf("filename = %s , wordcount = %d", subtemp->f_name, subtemp->wordcount);
                    subtemp = subtemp->sublink;
                }
                return 1;
            }
            temp = temp->mainlink;
        }
    }
    printf("word not found\n");
}