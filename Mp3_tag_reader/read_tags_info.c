#include "main.h"

const char *id3v1_genres[] = {
    "Blues", "Classic Rock", "Country", "Dance", "Disco", "Funk", "Grunge", "Hip-Hop", "Jazz",
    "Metal", "New Age", "Oldies", "Other", "Pop", "R&B", "Rap", "Reggae", "Rock", "Techno",
    "Industrial", "Alternative", "Ska", "Death Metal", "Pranks", "Soundtrack", "Euro-Techno",
    "Ambient", "Trip-Hop", "Vocal", "Jazz+Funk", "Fusion", "Trance", "Classical", "Instrumental",
    "Acid Punk", "Acid Jazz", "Polka", "Retro", "Rock & Roll", "Hard Rock", "Folk", "Folk Rock",
    "National Folk", "Swing", "Fast Fusion", "Bebop", "Latin", "Revival", "Celtic", "Bluegrass",
    "Avantgarde", "Gothic Rock", "Progressive Rock", "Psychedelic Rock", "Slow Rock", "Big Band",
    "Chorus", "Easy Listening", "Acoustic", "Humour", "Speech", "Chanson", "Opera", "Art Rock",
    "Progressive House", "Psytrance", "Chillout", "Indie", "Britpop", "Big Beat", "Breakbeat",
    "DJ", "House", "Trance", "Eurodance", "Dream", "Southern Rock", "Comedy", "Cult", "Gangsta",
    "Top 40", "Christian Rap", "Pop/Funk", "Jungle", "Native American", "Cabaret", "New Wave",
    "Punk Rock", "Drum Solo", "Acapella", "Eurodance", "Alternative Rock", "Polka", "Indie Rock",
    "Grunge Rock"};

int validate_tag(char *tag)
{
    char *str[] = {"TIT2", "TPE1", "TALB", "TYER", "TCON", "COMM"};

    int found = 0;

    for (int i = 0; i < 6; i++)
    {
        if (strcmp(str[i], tag) == 0)
        {
            found = 1;
            break;
        }
    }

    if (found == 1)
    {
        return SUCCESS;
    }
    else
    {
        return FAILURE;
    }
}
void print_tag(char *tag)
{
    // Print the valid title

    if (strcmp(tag, "TIT2") == 0)
    {

        printf("TITLE\t:\t");
    }
    else if (strcmp(tag, "TPE1") == 0)
    {

        printf("ARTIST\t:\t");
    }
    else if (strcmp(tag, "TALB") == 0)
    {

        printf("ALBUM\t:\t");
    }
    else if (strcmp(tag, "TYER") == 0)
    {

        printf("YEAR\t:\t");
    }
    else if (strcmp(tag, "TCON") == 0)
    {

        printf("MUSIC\t:\t");
    }
    else if (strcmp(tag, "COMM") == 0)
    {

        printf("LAN\t:\t");
    }
}
int read_id3v2(metadata *viewInfo)
{

    int offset = 10, size = 0;
    char title_buffer[5];
    char size_buffer[5];
    char metadata[2];
    FILE *fptr = fopen(viewInfo->file_name, "rb");
    if (fptr == NULL)
    {
        perror("Failed to open file");
        return FAILURE;
    }

    if (fread(metadata, 1, 10, fptr) != 10)
    {
        printf("Failed to read ID3v2 header\n");
        fclose(fptr);
        return FAILURE;
    }
    viewInfo->tag_version_major = metadata[0];
    viewInfo->tag_version_minor = metadata[1];

    // Ensure the application does not support ID3v2.4
    if (viewInfo->tag_version_major == 4)
    {
        printf("This file contains ID3v2.4, which is not supported by this application.\n");
        fclose(fptr);
        return FAILURE;
    }
    fclose(fptr);
    printf("-----------------------------------------------------------------------------------------------------------------------\n");
    printf("\t\t\tSONG: %s\t[%ld MB]\n", viewInfo->file_name, viewInfo->size);
    printf("-----------------------------------------------------------------------------------------------------------------------\nTAG\t:\t%s\n", viewInfo->tag);
    for (int i = 0; i < 6; i++)
    {
        fseek(viewInfo->mp3_fptr, offset, SEEK_SET);

        // Read the title (4 characters)
        fread(title_buffer, 4, 1, viewInfo->mp3_fptr);
        title_buffer[4] = '\0'; // null termination

        // Read the size even if the tag is invalid
        fread(size_buffer, 4, 1, viewInfo->mp3_fptr);

        // Converting size from Big Endian to Little Endian
        for (int i = 0; i < 2; i++)
        {
            char temp = size_buffer[i];
            size_buffer[i] = size_buffer[4 - i - 1];
            size_buffer[4 - i - 1] = temp;
        }

        // Copying the size to an integer variable
        memcpy(&size, size_buffer, sizeof(size));
        size = size - 1; // Adjust the size if necessary

        // Validate the tag before proceeding
        if (validate_tag(title_buffer) == FAILURE)
        {
            // Skip the current frame data if the tag is invalid
            offset += (11 + size); // Move to the next frame (skip current frame data and header)
            continue;
        }

        print_tag(title_buffer);

        // Read the frame data into new_str
        char new_str[size + 1]; // +1 for null terminator
        // offset + 11, in 11 we have 4(TIT2)+4(size)+3(flag)
        fseek(viewInfo->mp3_fptr, offset + 11, SEEK_SET); // Move to the data portion of the frame
        fread(new_str, size, 1, viewInfo->mp3_fptr);
        new_str[size] = '\0'; // Null terminate the string

        // Replace any null characters with dots (.)
        for (int i = 0; i < size; i++)
        {
            if (new_str[i] == '\0')
            {
                new_str[i] = ' ';
            }
        }

        // Print the valid frame data
        printf("%s\n", new_str);

        // Update offset for the next frame
        offset += (11 + size);
    }
}

int fetch_data(metadata *file_info)
{
    char metadata[128] = {0};
    // printf("hey\n");
    int tag = find_tag(file_info);

    if (tag == ID3V1)
    {

        FILE *fptr = fopen(file_info->file_name, "r");
        if (fptr == NULL)
        {
            perror("Failed to open file");
            return FAILURE;
        }

        fseek(fptr, 0, SEEK_END);
        file_info->size = ((float)ftell(fptr) / MB);
        // printf("%.1ld mb\n", file_info->size);
        if (fseek(fptr, -128, SEEK_END) != 0)
        {
            printf("Error in fseek\n");
            fclose(fptr);
            return FAILURE;
        }
        size_t readed = fread(metadata, sizeof(char), 128, fptr);

        if (readed < 128)
        {
            printf("error\n");
            return FAILURE;
        }
        if (strncmp(metadata, "TAG", 3) != 0)
        {
            printf("ID3V1 tag not found\n");
            return FAILURE;
        }
        file_info->tag = malloc(5);
        strcpy(file_info->tag, "ID3v1");

        // printf("The %s  have ID3V1 tag\n", file_info->file_name);
        file_info->title = malloc(31);
        strncpy(file_info->title, metadata + 3, 30);
        file_info->title[30] = '\0';
        // printf("%s\n", file_info->title);

        // populating artist
        file_info->artist = malloc(31);
        strncpy(file_info->artist, metadata + 3 + 30, 30);
        file_info->artist[30] = '\0';
        // printf("%s\n", file_info->artist);

        // populating album
        file_info->album = malloc(31);
        strncpy(file_info->album, metadata + 3 + 30 + 30, 30);
        file_info->album[30] = '\0';
        // printf("%s\n", file_info->album);

        // populationg year
        file_info->year = malloc(5);
        strncpy(file_info->year, metadata + 3 + 30 + 30 + 30, 4);
        file_info->year[4] = '\0';
        // printf("%s\n", file_info->year);

        // populating comment
        file_info->comment = malloc(31);
        strncpy(file_info->comment, metadata + 3 + 30 + 30 + 30 + 4, 30);
        file_info->comment[30] = '\0';
        // printf("%s\n", file_info->comment);

        file_info->genre = metadata[127];
        if (file_info->genre < 0 || file_info->genre > 147)
        {
            file_info->genre_name = strdup("Unknown");
        }
        else
        {
            file_info->genre_name = strdup(id3v1_genres[file_info->genre]);
        }

        // printf("%c\n", file_info->genre);
        return SUCCESS;
    }
    else if (tag == ID3V2)
    {

        FILE *fptr = fopen(file_info->file_name, "rb");
        if (fptr == NULL)
        {
            perror("Failed to open file");
            return FAILURE;
        }
        file_info->tag = malloc(5);
        strcpy(file_info->tag, "ID3v2");
        // strcpy(file_info->mp3_fptr, fptr);
        file_info->mp3_fptr = fptr;
        fseek(fptr, 0, SEEK_END);
        file_info->size = ((float)ftell(fptr) / MB);
        fseek(fptr, 0, SEEK_SET);
        read_id3v2(file_info);
        // return SUCCESS;
    }
    else
    {
        printf("No valid ID3 tag found\n");
        return FAILURE;
    }
}
