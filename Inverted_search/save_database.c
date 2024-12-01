#include "main.h"
/*
Format for saving
#<index_no.s>;
<word>;<file_cout>;<file_name>;<word_count>#

*/
int save_database(main_ **arr, char *f_name)
{
    FILE *fp = fopen(f_name, "w");

    for (int i = 0; i < 26; i++) // from 0 to 26 index
    {
        if (arr[i] != NULL)
        {

            // printf("word : %s,filecount : %d", temp->word, temp->filecount);
            // wrtie a output instead of terminal to file
            fprintf(fp, "\n#<%d>;\n", i);
            main_ *temp = arr[i]; // main node
            while (temp)
            {
                fprintf(fp, "<%s>;<%d>;", temp->word, temp->filecount);
                sub *subtemp = temp->sublink;
                while (subtemp)
                {
                    // printf("filename = %s , wordcount = %d", subtemp->f_name, subtemp->wordcount);
                    fprintf(fp, "<%s>;<%d>;", subtemp->f_name, subtemp->wordcount);
                    subtemp = subtemp->sublink;
                }
                temp = temp->mainlink;
            }
        }
    }
    fclose(fp);
    printf("-------DATABASE Saved Successfully------\n");
    return 0;
}