#include "main.h"
#include <stdio.h>
int display_database(main_ **arr)
{
    for (int i = 0; i < 26; i++) // from 0 to 26 index
    {
        if (arr[i] == NULL)
        {
            continue;
        }
        else
        {
            main_ *temp = arr[i]; // main node
            while (temp)
            {
                printf("\nword : %s ,filecount : %d ,", temp->word, temp->filecount);
                sub *subtemp = temp->sublink;
                while (subtemp)
                {
                    printf("filename = %s , wordcount = %d ,", subtemp->f_name, subtemp->wordcount);
                    subtemp = subtemp->sublink;
                }
                temp = temp->mainlink;
            }
        }
    }
}