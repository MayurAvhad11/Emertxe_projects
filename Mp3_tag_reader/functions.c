
#include "main.h"

void free_metadata(metadata *file_info)
{
    free(file_info->title);
    free(file_info->artist);
    free(file_info->album);
    free(file_info->year);
    free(file_info->comment);
    free(file_info->tag);
}
void display_help()
{
    printf("\t\tTO VIEW\t\nUsage: ./mp3tagreader filename.mp3\n\n");
    printf("\t\tTO EDIT\t\nUsage: ./mp3tagreader [options] filename\n");
    printf("Options:\n");
    printf("-t\t\t\tModify Title\n-a\t\t\tModify Artist\n-A\t\t\tModify Album\n-y\t\t\tModify Year\n-c\t\t\tModify comment\n-g\t\t\tModify Genre\n");
}

int validate_file(char *argv[], metadata *file_info, int argc)
{
    if (argc < 2 || argc > 3)
    {
        return FAILURE;
    }

    char *file = (argc == 2) ? argv[1] : argv[2];
    char *ext = strrchr(file, '.');
    if (ext && strcmp(ext, ".mp3") == 0)
    {
        file_info->file_name = file;
        return SUCCESS;
    }

    return FAILURE;
}

int find_tag(metadata *file_info)
{
    char metadata[10] = {0};
    FILE *fptr = fopen(file_info->file_name, "r");
    if (fptr == NULL)
    {
        perror("Failed to open file");
        return FAILURE;
    }

    // Check for ID3v2 tag
    fread(metadata, 1, 3, fptr);
    // printf("%s\n", metadata);
    if (strncmp(metadata, "ID3", 3) == 0)
    {
        fclose(fptr);
        return ID3V2;
    }

    // Check for ID3v1 tag
    fseek(fptr, -128, SEEK_END);
    fread(metadata, 1, 3, fptr);
    fclose(fptr);
    if (strncmp(metadata, "TAG", 3) == 0)
    {
        return ID3V1;
    }

    return FAILURE;
}

void display_tag(metadata *file_info)
{
    printf("-----------------------------------------------------------------------------------------------------------------------\n");
    printf("\t\t\tSONG: %s\t[%ld MB]\n", file_info->file_name, file_info->size);
    printf("-----------------------------------------------------------------------------------------------------------------------\n");

    printf("TAG\t:\t%s\nTITLE\t:\t%s\nARTIST\t:\t%s\nALBUM\t:\t%s\nYEAR\t:\t%s\nGENRE\t:\t%s\nCOMMENT\t:\t%s\n", file_info->tag, file_info->title, file_info->artist, file_info->album, file_info->year, file_info->genre_name, file_info->comment);
    free_metadata(file_info);
}
