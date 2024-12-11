#include "main.h"

int modify_tag(char *argv[], metadata *file_info)
{
    char *modify_options[] = {"-t", "-a", "-A", "-y", "-c", "-g"};
    char *frame_ids[] = {"TIT2", "TPE1", "TALB", "TYER", "COMM", "TCON"};
    char *msg[] = {"Title", "Artist", "Album", "Year", "Comment", "Genre"};
    int flag = -1;

    for (int i = 0; i < 6; i++)
    {
        if (strcmp(argv[1], modify_options[i]) == 0)
        {
            flag = i;
            break;
        }
    }

    if (flag == -1)
    {
        display_help();
        return FAILURE;
    }

    printf("Enter new %s:\n", msg[flag]);
    char new_value[256] = {0};
    scanf(" %[^\n]", new_value);
    int tag = find_tag(file_info);
    if (tag == ID3V1)
    {
        if (modify_tag_v1(file_info, new_value, argv) == SUCCESS)
        {

            return SUCCESS;
        }
    }
    if (tag == ID3V2)
    {

        if (update_id3v2_tag(file_info->file_name, frame_ids[flag], new_value) == SUCCESS)
        {
            printf("%s updated successfully\n", msg[flag]);
            return SUCCESS;
        }
    } //(metadata *file_info, char *user_input, char *argv[], const char *frame_id)
    return FAILURE;
}