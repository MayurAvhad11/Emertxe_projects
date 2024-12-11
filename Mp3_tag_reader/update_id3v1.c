#include "main.h"

int write_tag(FILE *fptr, long offset, const char *user_input, size_t field_size)
{
    if (fseek(fptr, offset, SEEK_END) != 0)
    {
        printf("Error seeking to field\n");
        return FAILURE;
    }
    char field_data[field_size];
    memset(field_data, 0, field_size);
    strncpy(field_data, user_input, field_size);
    size_t written = fwrite(field_data, sizeof(char), field_size, fptr);

    if (written < field_size)
    {
        printf("Error writting to field\n");
        return FAILURE;
    }
    return SUCCESS;
}
int modify_tag_v1(metadata *file_info, char *user_input, char *argv[])
{
    // int len = strlen(user_input);
    char metadata[128] = {0};
    char tag[4] = {0};
    FILE *fptr = fopen(file_info->file_name, "r+");
    if (fptr == NULL)
    {
        return FAILURE;
    }

    fseek(fptr, 0, SEEK_END);
    file_info->size = ((float)ftell(fptr) / MB);
    // printf("%.1ld mb\n", file_info->size);
    if (fseek(fptr, -128, SEEK_END) != 0)
    {
        printf("Error in fseek\n");
    }
    size_t readed = fread(metadata, sizeof(char), 128, fptr);

    if (readed < 128)
    {
        printf("error\n");
        return FAILURE;
    }
    strncpy(tag, metadata, 3);
    // tag[3] = '\0';
    if (strcmp(tag, "TAG") != 0)
    {

        printf("The %s dont have ID3V1 tag\n", file_info->file_name);
        fclose(fptr);
        return FAILURE;
    }
    const long offsets[] = {
        -128 + 3,                        // Title
        -128 + 3 + 30,                   // Artist
        -128 + 3 + 30 + 30,              // Album
        -128 + 3 + 30 + 30 + 30,         // Year
        -128 + 3 + 30 + 30 + 30 + 4,     // Comment
        -128 + 3 + 30 + 30 + 30 + 4 + 30 // Genre
    };
    const size_t sizes[] = {30, 30, 30, 4, 30, 1};
    char *modify_options[] = {"-t", "-a", "-A", "-y", "-c", "-g"};
    char *msg[] = {"Title", "Artist", "Album", "Year", "comment", "Genere"};
    for (int i = 0; i < 6; i++)
    {
        if (strcmp(argv[1], modify_options[i]) == 0)
        {
            if (write_tag(fptr, offsets[i], user_input, sizes[i]) == FAILURE)
            {
                fclose(fptr);
                return FAILURE;
            }
            printf("%s updated Successfully\n", msg[i]);
        }
    }

    // printf("%c\n", file_info->genre);
    return SUCCESS;
}