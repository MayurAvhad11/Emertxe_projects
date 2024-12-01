#include "main.h"

int create_database(main_ **arr, file_node *head)
{
    char word[20];
    int index, flag;
    // open file one by one from linked list
    while (head)
    {
        FILE *fp = fopen(head->filename, "r");
        // collect words from files

        while (fscanf(fp, "%s", word) != EOF)
        {
            // generate index for words
            index = tolower(word[0]) % 97;
            flag = 0; // Initialize flag
            // check if link is == NULL
            if (arr[index] == NULL) // means its first node
            {
                // update the info
                main_ *mnew = malloc(sizeof(main_));
                mnew->filecount = 1;
                strcpy(mnew->word, word);
                mnew->mainlink = NULL;
                // create a sub node
                sub *snew = malloc(sizeof(sub));
                snew->wordcount = 1;
                strcpy(snew->f_name, head->filename);
                snew->sublink = NULL;
                mnew->sublink = snew;

                arr[index] = mnew;
            }
            else // means there is already a node present
            {
                main_ *mt = arr[index];
                main_ *prev_main = NULL;
                while (mt)
                {
                    if (strcmp(mt->word, word) != 0) // the word is not same
                    {
                        prev_main = mt;
                        mt = mt->mainlink;
                        flag = 1;
                    }
                    else // if the words are matching then check if its in same file or not
                    {
                        // checking file name is same or not
                        sub *sub_temp = mt->sublink;
                        sub *pre_subtemp = NULL;
                        while (sub_temp)
                        {
                            if (strcmp(sub_temp->f_name, head->filename) == 0) // cheking file name of opend file and sub node file are same or not
                            {
                                // same word from same file , inc only word count
                                sub_temp->wordcount++;
                                flag = 0;
                                break;
                            }
                            pre_subtemp = sub_temp;
                            sub_temp = sub_temp->sublink; // if its not same check in another sub node
                        }
                        if (flag == 1)
                        {
                            // filename is not matching , create new sub node and attach it to prev sub nod
                            sub *snew = malloc(sizeof(sub));
                            snew->wordcount = 1;
                            strcpy(snew->f_name, head->filename);
                            snew->sublink = NULL;
                            pre_subtemp->sublink = snew;
                            mt->filecount++;
                        }
                        flag = 0;
                        break;
                    }
                }

                if (flag == 1)
                {
                    // create new node and attach it with prev node
                    main_ *mnew = (main_ *)malloc(sizeof(main_));
                    prev_main->mainlink = mnew;
                    // update the info

                    mnew->filecount = 1;
                    strcpy(mnew->word, word);

                    // create a sub node
                    sub *snew = malloc(sizeof(sub));
                    snew->wordcount = 1;
                    strcpy(snew->f_name, head->filename);
                    snew->sublink = NULL;
                    mnew->sublink = snew;
                    // mnew->mainlink = NULL;
                    // arr[index] = mnew;
                }
            }
        }
        fclose(fp);
        head = head->link;
    }
    printf("Database successfully created\n");
    return 0;
}