#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// to store the file name as list:
typedef struct filename
{
    char filename[30];
    struct filename *link;
} file_node;

// struct to store file names
typedef struct file
{
    int wordcount;
    char f_name[20];
    struct file *sublink;
} sub;

// to store word details
typedef struct word
{
    char word[15];
    int filecount;
    struct word *mainlink;
    struct file *sublink;
} main_;

// prototypes
int validation(file_node **head, char *filename[], int argc);

int create_database(main_ **arr, file_node *head);

int search_database(main_ **arr, char *search_word);

int display_database(main_ **arr);

int save_database(main_ **arr, char *f_name);

int update_database(file_node **head, main_ **arr, char *filename);