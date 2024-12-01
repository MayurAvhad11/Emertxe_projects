#ifndef DECODE_H
#define DECODE_H

#include "types.h"
#include "string.h"

#define MAX_SECRET_BUF_SIZE 1
#define MAX_IMAGE_BUF_SIZE (MAX_SECRET_BUF_SIZE * 8)
#define MAX_FILE_SUFFIX 4
typedef struct _DecodeInfo
{
    // stego file info
    char *stego_fname;
    FILE *fptr_stego;
    char decode_magic[2];
    char stego_image_data[MAX_IMAGE_BUF_SIZE];
    // output file
    char *output_fname;
    char extn_sec_file[MAX_FILE_SUFFIX];
    FILE *fptr_output;

    // decoded data(For future changes)
    char *secret_file_extn;
    char *secret_file_data;
    unsigned int secret_file_size;
    unsigned int secret_file_ext_size;

} DecodeInfo;

// functions definations
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo);

// do encoding
Status do_decoding(DecodeInfo *decInfo);

// open files
Status open_files_decoding(DecodeInfo *decInfo);

// Status decode_magic_string(const char *magic_string, DecodeInfo *decInfo);
// decode the magic string
char *decode_magic_string(DecodeInfo *decInfo);

char decode_bytes_from_lsb(char data, char *stego_image_buffer);

char *decode_data_from_image(int size, FILE *fpter_stego, DecodeInfo *decInfo);
Status decode_secret_file_ext(DecodeInfo *decInfo);
unsigned int decode_size_from_lsb(char *stego_image_buffer);

Status decode_ext_size(FILE *fptr_stego, DecodeInfo *decInfo);
Status decode_secret_file_size(DecodeInfo *decInfo);
Status decode_secret_file_data(DecodeInfo *decInfo);
#endif