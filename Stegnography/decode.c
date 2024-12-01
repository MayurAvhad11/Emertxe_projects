
#include <stdio.h>
#include <stdlib.h>
#include "decode.h"
#include "types.h"
#include "common.h"

Status open_files_decoding(DecodeInfo *decInfo) // opens all the validated files
{
    // open stego file
    decInfo->fptr_stego = fopen(decInfo->stego_fname, "r");
    if (decInfo->fptr_stego == NULL) // error handling
    {
        perror("fopen");
        fprintf(stderr, "Error: unable to open file %s\n", decInfo->stego_fname);

        return d_failure;
    }

    // opeing output file
    decInfo->fptr_output = fopen(decInfo->output_fname, "w");
    if (decInfo->fptr_output == NULL)
    {
        perror("fopen");
        fprintf(stderr, "Error: unable to open file %s\n", decInfo->output_fname);
        return d_failure;
    }

    // no failure
    return d_success;
}
Status read_and_validate_decode_args(char *argv[], DecodeInfo *decInfo) // validtaed the user provide files
{
    // validate the arg == .bmp
    if (argv[2] != NULL && strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        decInfo->stego_fname = argv[2]; // storing the file name
    }
    else
    {
        return d_failure;
    }

    // check if provide output file
    if (argv[3] != NULL) // if user provide file name
    {
        decInfo->output_fname = argv[3];
    }
    else // if user dosent provide file name , use default
    {
        decInfo->output_fname = "output.txt";
    }
    return d_success;
}

char decode_bytes_from_lsb(char data, char *stego_image_buffer) // decodes bytes one by one from lsb of image file
{
    unsigned char decoded_data = 0;

    for (int i = 0; i < 8; i++)
    {
        decoded_data = decoded_data | ((stego_image_buffer[i] & 1) << (7 - i));
    }
    return decoded_data;
}

char *decode_data_from_image(int size, FILE *fpter_stego, DecodeInfo *decInfo) // decode data from the image
{
    // int size = 2;
    char *dec_string = (char *)malloc(size + 1);
    if (dec_string == NULL)
    {
        printf("Memory allocation failed\n");
        return NULL;
    }

    for (int i = 0; i < size; i++)
    {
        // read 8 bytes from stego file
        fread(decInfo->stego_image_data, 8, sizeof(char), decInfo->fptr_stego);
        // decode bytes from lsb one by one
        dec_string[i] = decode_bytes_from_lsb(0, decInfo->stego_image_data);
    }
    dec_string[size] = '\0'; // null terminating
    return dec_string;
}
char *decode_magic_string(DecodeInfo *decInfo) // decoding magic string from image
{
    // Assuming the magic string starts at an offset of 54 bytes
    fseek(decInfo->fptr_stego, 54, SEEK_SET);

    // Decode the magic string from the image
    char *decoded_string = decode_data_from_image(2, decInfo->fptr_stego, decInfo);

    if (decoded_string == NULL)
    {
        printf("Decoding magic string failed\n");
        return NULL;
    }

    return decoded_string; // Return the decoded magic string
}
unsigned int decode_size_from_lsb(char *stego_image_buffer)
{
    unsigned int decoded_data = 0;

    for (int i = 0; i < 32; i++)
    {
        // Extract the least significant bit and shift it to its correct position
        decoded_data = decoded_data | ((stego_image_buffer[i] & 1) << (31 - i));
    }

    return decoded_data; // Return the decoded size
}

Status decode_ext_size(FILE *fptr_stego, DecodeInfo *decInfo) // step 6 - decode secrete files extension size from image

{

    char str[32]; // for ext
    fread(str, sizeof(char), 32, fptr_stego);
    decInfo->secret_file_ext_size = decode_size_from_lsb(str); // decode & store it
    // printf("file extension size: is %d\n", decInfo->secret_file_ext_size);(dubugging purpose)
    return d_success;
}

Status decode_secret_file_ext(DecodeInfo *decInfo) // step 7
{

    char *file_extn = ".txt";                                                   // storing the .txt (for commparing)
    int size = 4;                                                               // size of .txt
    char *dec_ext = decode_data_from_image(size, decInfo->fptr_stego, decInfo); // decoding secrete file ext & stroing it to member
    if (dec_ext == NULL)
    {
        printf("Memory allocation failed\n");
        return d_failure;
    }
    decInfo->secret_file_extn = dec_ext;
    // printf("Decoded file extension: %s\n", dec_ext);
    if (strcmp(dec_ext, file_extn) == 0) // comapring
    {
        free(dec_ext);
        return d_success;
    }

    return d_failure;
}

Status decode_secret_file_size(DecodeInfo *decInfo)
{
    unsigned int file_size = 0;
    char str[32]; // to store size
    fread(str, sizeof(char), 32, decInfo->fptr_stego);
    file_size = decode_size_from_lsb(str);
    decInfo->secret_file_size = file_size;
    // printf("The secret file size is %d\n", file_size);
    return d_success;
}

Status decode_secret_file_data(DecodeInfo *decInfo) // decoding the hidden data from image
{
    decInfo->secret_file_data = (char *)malloc(decInfo->secret_file_size + 1); // allocating size of secret file to secrete_file_data
    if (decInfo->secret_file_data == NULL)                                     // error handling
    {
        printf("Failed memory allocation for secret data\n");
        return d_failure;
    }

    for (int i = 0; i < decInfo->secret_file_size; i++) // run loop till sec file size
    {
        fread(decInfo->stego_image_data, 1, 8, decInfo->fptr_stego);                        // reading from stego & storing to image_data
        decInfo->secret_file_data[i] = decode_bytes_from_lsb(0, decInfo->stego_image_data); // decoding one by one  and storng to secre_file_data
    }
    decInfo->secret_file_data[decInfo->secret_file_size] = '\0';                                      // putting null termination at end
    fwrite(decInfo->secret_file_data, sizeof(char), decInfo->secret_file_size, decInfo->fptr_output); // writing the decoded data(secret_file_data) to output file
    return d_success;
}

// step 3
Status do_decoding(DecodeInfo *decInfo)
{

    // open all the required files
    if (open_files_decoding(decInfo) == d_success)
    {
        printf("Successfully opened all the requried files\n");
        // start encoding
        // encode magic string
        if (strcmp(decode_magic_string(decInfo), MAGIC_STRING) == 0)
        {
            // printf("this is string :%s\n", decode_magic_string(decInfo));
            printf("Successfully decoded magic string(secrete data is present)\n");

            if (decode_ext_size(decInfo->fptr_stego, decInfo) == d_success)
            {
                printf("Successfully decoded secrte file extension size\n");

                if (decode_secret_file_ext(decInfo) == d_success)
                {
                    printf("Decoded secret file extension successfully\n");
                    if (decode_secret_file_size(decInfo) == d_success)
                    {
                        printf("Decoded secret file size successfully\n");
                        if (decode_secret_file_data(decInfo) == d_success)
                        {
                            printf("Decoded secret msg from file successfully\n");
                        }
                        else
                        {
                            printf("Failed to decode secret msg from file\n");
                            return d_failure;
                        }
                    }
                    else
                    {
                        printf("Failed to decode secret file size\n");
                        return d_failure;
                    }
                }
                else
                {
                    printf("Failed to decode secret file extension\n");
                    return d_failure;
                }
            }
            else
            {
                printf("Failed to decode secrete file extension size\n");
                return d_failure;
            }
        }
        else
        {

            printf("No Secrete data in the image file\n");
            return d_failure;
        }
    }
    else
    {
        printf("Failed to open files\n");
        return d_failure;
    }
    return d_success;
}