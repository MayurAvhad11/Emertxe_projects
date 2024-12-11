#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stdlib.h>
#include <ctype.h>

#define HEADER_SIZE 10
#define FRAME_HEADER_SIZE 10
#define SUCCESS 0
#define FAILURE -1
#define MB (1024 * 1024)
#define ID3V1 10
#define ID3V2 20

// Frame structure to manage parsed frames
typedef struct
{
    char id[5];    // Frame ID
    int size;      // Size of frame content
    char *content; // Frame content
} Frame;

typedef struct metadata
{
    char *tag;
    char *title;   // Variable-length, stores TIT2 (Title)
    char *artist;  // Variable-length, stores TPE1 (Artist)
    char *album;   // Variable-length, stores TALB (Album)
    char *year;    // Variable-length, stores TYER (Year) if present
    char *comment; // Variable-length, stores COMM (Comment)
                   // char *genre;   // 1byte
    unsigned char genre;

    char *genre_name;
    FILE *mp3_fptr;
    // Variable-length, stores TCON (Genre)

    char *track_number; // Variable-length, stores TRCK (Track Number)
    char *file_name;    // Stores the file name of the MP3
    size_t size;        // File size

    unsigned char tag_version_major; // Stores major version (e.g., 3 for ID3v2.3)
    unsigned char tag_version_minor; // Stores minor version (e.g., 0 for ID3v2.3.0)
} metadata;

// Genre table for ID3v1 (predefined)

int validate_file(char *argv[], metadata *file_info, int argc);
int find_tag(metadata *file_info);
int modify_tag_v1(metadata *file_info, char *user_input, char *argv[]);
void display_tag(metadata *tag_info);
void display_help();
void free_metadata(metadata *file_info);
int modify_tag(char *argv[], metadata *file_info);
int write_tag(FILE *fptr, long offset, const char *user_input, size_t field_size);
int fetch_data(metadata *file_info);
int modify_tag_v2(metadata *file_info, char *user_input, char *argv[], const char *frame_id);
int update_id3v2_tag(const char *filename, const char *tag, const char *new_value);
int reconstruct_tag(unsigned char **tag_data, Frame *frames, int frame_count);
int parse_frames(const unsigned char *tag_data, int tag_size, Frame **frames);
void write_syncsafe_int(unsigned char *data, int value);
int read_syncsafe_int(const unsigned char *data);
int validate_tag(char *tag);
#endif