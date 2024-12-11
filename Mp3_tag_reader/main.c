#include "main.h"

int main(int argc, char *argv[])
{
    if (argc < 2 || argc > 3)
    {
        display_help();
        return FAILURE;
    }

    metadata file_info;

    if (validate_file(argv, &file_info, argc) == SUCCESS)
    {
        if (find_tag(&file_info) == ID3V1)
        {
            if (argc == 2)
            {
                if (fetch_data(&file_info) == SUCCESS)
                {
                    display_tag(&file_info);
                }
                else
                {
                    printf("The file has id3v2 tag\n");
                }
            }
            else if (argc == 3)
            {
                if (modify_tag(argv, &file_info) == FAILURE)
                {
                    printf("Failed to modify tag\n");
                }
            }
        }
        else if (find_tag(&file_info) == ID3V2)
        {
            if (argc == 2)
            {
                if (fetch_data(&file_info) == SUCCESS)
                {
                    display_tag(&file_info);
                    // printf("hello\n");
                }
                else
                {
                    printf("\n");
                }
            }
            if (argc == 3)
            {
                if (modify_tag(argv, &file_info) == FAILURE)
                {
                    printf("Failed to modify tag\n");
                }
            }
        }
        else
        {
            printf("no tag found in this file\n");
        }
    }
    else
    {
        printf("Please provide a valid mp3 file.\n");
        printf("USAGE: ./mp3_tag_reader sample.mp3\n");
        return FAILURE;
    }

    return 0;
}
