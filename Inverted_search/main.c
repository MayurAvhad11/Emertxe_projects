#include "main.h"

int main(int argc, char *argv[])
{

    if (argc > 1)
    {

        // array of pointers
        main_ *arr[26] = {NULL};
        // to store file filname as list
        file_node *head = NULL;

        validation(&head, argv, argc);
        int choice;
        char ch;
        // show menu & prompt for user
        do
        {
            printf("1.create\n2.Display\n3.Search\n4.Save\n5.Update\n");
            scanf("%d", &choice);
            switch (choice)
            {
            case 1:
            {
                // call create function
                create_database(arr, head);
                break;
            }
            case 2:
            {
                // call dispaly function
                display_database(arr);
                break;
            }
            case 3:
            {
                // call search function
                char word[20];
                printf("enter a word to search: ");
                scanf("%s", word);
                search_database(arr, word);
                break;
            }
            case 4:
            {
                // call save function
                char backup_fname[20];
                printf("Enter backup file name: ");
                scanf("%s", backup_fname);
                save_database(arr, backup_fname);
                break;
            }
            case 5:
            {
                // call update function
                char fname[20];
                printf("Enter the file name to update data: ");
                scanf("%s", fname);
                // Call update function
                update_database(&head, arr, fname);
                break;
                        }
            }
            printf("\n");
            printf("Do you want to continue(y/n):\n");
            printf("Enter you choice: ");
            scanf(" %c", &ch);

        } while (ch == 'y' || ch == 'Y');
    }
    else
    {
        printf("please pass some file\n");
        return 1;
    }
}