#include <stdio.h>
#include "encode.h"
#include "types.h"
#include "common.h"

/* Function Definitions */

/* Get image size
 * Input: Image file ptr
 * Output: width * height * bytes per pixel (3 in our case)
 * Description: In BMP Image, width is stored in offset 18,
 * and height after that. size is 4 bytes
 */
uint get_image_size_for_bmp(FILE *fptr_image)
{
    uint width, height;
    // Seek to 18th byte
    fseek(fptr_image, 18, SEEK_SET);

    // Read the width (an int)
    fread(&width, sizeof(int), 1, fptr_image);
    printf("width = %u\n", width);

    // Read the height (an int)
    fread(&height, sizeof(int), 1, fptr_image);
    printf("height = %u\n", height);

    // Return image capacity
    return width * height * 3;
}

/*
 * Get File pointers for i/p and o/p files
 * Inputs: Src Image file, Secret file and
 * Stego Image file
 * Output: FILE pointer for above files
 * Return Value: e_success or e_failure, on file errors
 */
Status open_files(EncodeInfo *encInfo)
{
    // Src Image file
    encInfo->fptr_src_image = fopen(encInfo->src_image_fname, "r");
    // Do Error handling
    if (encInfo->fptr_src_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->src_image_fname);

        return e_failure;
    }

    // Secret file
    encInfo->fptr_secret = fopen(encInfo->secret_fname, "r");
    // Do Error handling
    if (encInfo->fptr_secret == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->secret_fname);

        return e_failure;
    }

    // Stego Image file
    encInfo->fptr_stego_image = fopen(encInfo->stego_image_fname, "w");
    // Do Error handling
    if (encInfo->fptr_stego_image == NULL)
    {
        perror("fopen");
        fprintf(stderr, "ERROR: Unable to open file %s\n", encInfo->stego_image_fname);

        return e_failure;
    }

    // No failure return e_success
    return e_success;
}

// validation of bmp image file
Status read_and_validate_encode_args(char *argv[], EncodeInfo *encInfo)
{
    // validate the sec arg == .bmp
    if (argv[2] != NULL && strcmp(strstr(argv[2], "."), ".bmp") == 0)
    {
        encInfo->src_image_fname = argv[2]; // store the file name(.bmp) in pointer
    }
    else // no .bmp
    {
        return e_failure;
    }

    // validating the third arg == .txt
    if (argv[3] != NULL && strcmp(strstr(argv[3], "."), ".txt") == 0)
    {

        encInfo->secret_fname = argv[3]; // store the file name(.txt) in pointer
    }
    else // no .txt
    {
        return e_failure;
    }

    // store the output image file name
    if (argv[4] != NULL) // if user provided the file name
    {
        encInfo->stego_image_fname = argv[4];
    }
    else
    {
        encInfo->stego_image_fname = "stego.bmp";
    }
    return e_success;
}

uint get_file_size(FILE *fptr_secret)
{
    fseek(fptr_secret, 0, SEEK_END); // bring the pointer to start
    return ftell(fptr_secret);
}

// geting size of image file and secrete file
Status check_capacity(EncodeInfo *encInfo)
{
    encInfo->image_capacity = get_image_size_for_bmp(encInfo->fptr_src_image);
    encInfo->size_secret_file = get_file_size(encInfo->fptr_secret);
    if (encInfo->image_capacity > (54 + 16 + 32 + 32 + 32 + (encInfo->size_secret_file * 8)))
    {
        return e_success;
    }
    else
    {
        return e_failure;
    }
}

// copy the header of bmp to stego
Status copy_bmp_header(FILE *fptr_src, FILE *fptr_stego)
{
    char header[54];                              // to store the header
    rewind(fptr_src);                             // bringing the ptr to starting point
    fread(header, sizeof(char), 54, fptr_src);    // reading the 54 bytes at ones
    fwrite(header, sizeof(char), 54, fptr_stego); // wrinteing 54 bytes to stego at ones
    return e_success;
}

// changing (encoding) the lsb of image file to encode magic str
Status encode_byte_to_lsb(char data, char *image_buffer)
{
    unsigned char mask = 1 << 7;
    for (int i = 0; i < 8; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((data & mask) >> (7 - i));
        mask = mask >> 1;
    }
    return e_success;
}
// function which encodes data to stgo_image
Status encode_data_to_image(const char *data, int size, FILE *fptr_src, FILE *fptr_stego, EncodeInfo *encInfo)
{
    // encode charcter by charcter (#-> 1st)

    for (int i = 0; i < size; i++)
    {
        // read 8 bytes of rbg from source file(for #)
        fread(encInfo->image_data, 8, sizeof(char), fptr_src);

        // encode byte to lsb(of source file)
        encode_byte_to_lsb(data[i], encInfo->image_data);
        fwrite(encInfo->image_data, 8, sizeof(char), fptr_stego); // whatever the encoded data(of image file) write it to stego file
    }
}

// encoding of magic string
Status encode_magic_string(const char *magic_str, EncodeInfo *encInfo)
{
    encode_data_to_image(magic_str, strlen(magic_str), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
    return e_success;
}

Status encode_size_to_lsb(char *image_buffer, int size)
{
    unsigned int mask = 1 << 31;
    for (int i = 0; i < 32; i++)
    {
        image_buffer[i] = (image_buffer[i] & 0xFE) | ((size & mask) >> (31 - i));
        mask = mask >> 1;
    }
    return e_success;
}

Status encode_size(int size, FILE *fptr_src, FILE *fptr_stego) // step 6
{
    char str[32]; // for extnsion -> .txt
    fread(str, sizeof(char), 32, fptr_src);
    encode_size_to_lsb(str, size);
    fwrite(str, sizeof(char), 32, fptr_stego);
    return e_success;
}

Status encode_secret_file_extn(const char *file_extn, EncodeInfo *encInfo) // step 7
{
    file_extn = ".txt"; // change this if secret file have different extension

    encode_data_to_image(file_extn, strlen(file_extn), encInfo->fptr_src_image, encInfo->fptr_stego_image, encInfo);
    return e_success;
}
Status encode_secret_file_size(long size, EncodeInfo *encInfo) // step 8
{
    char str[32]; // for extnsion -> .txt
    fread(str, sizeof(char), 32, encInfo->fptr_src_image);
    encode_size_to_lsb(str, size);
    fwrite(str, sizeof(char), 32, encInfo->fptr_stego_image);
    return e_success;
}

Status encode_secret_file_data(EncodeInfo *encInfo) // step 9
{
    char ch;
    // printf("%ld\n",ftell(encInfo->fptr_secret));
    fseek(encInfo->fptr_secret, 0, SEEK_SET); // bring pointer to 0

    for (int i = 0; i < encInfo->size_secret_file; i++)
    {
        fread(&ch, 1, sizeof(char), encInfo->fptr_secret);                       // reading secret data from the secrect file
        fread(encInfo->image_data, 8, sizeof(char), encInfo->fptr_src_image);    // reading bytes from img
        encode_byte_to_lsb(ch, encInfo->image_data);                             // encoding the data within image
        fwrite(encInfo->image_data, 8, sizeof(char), encInfo->fptr_stego_image); // writing data to stego
    }
    return e_success;
}
Status copy_remaining_img_data(FILE *fptr_src, FILE *fptr_stego)
{
    char ch;
    while (fread(&ch, 1, 1, fptr_src) > 0)
    {
        fwrite(&ch, 1, 1, fptr_stego);
    }
    return e_success;
}
// step 3: do encoding
Status do_encoding(EncodeInfo *encInfo)
{
    // call rest of the endcoding functions
    // open all the files(.bmp,.txt,and stego)
    if (open_files(encInfo) == e_success)
    {
        printf("Successfully opend all requried files\n");
        if (check_capacity(encInfo) == e_success)
        {
            printf("Encoding is possible\n");
            if (copy_bmp_header(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
            {
                printf("copied header successfully\n");

                // start the encoding
                if (encode_magic_string(MAGIC_STRING, encInfo) == e_success)
                {
                    printf("successfully encoded magic string\n");
                    if (encode_size(strlen(".txt"), encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success) // change here if using other extensions
                    {
                        printf("Successfully encoded file extension size successfully\n");
                        if (encode_secret_file_extn(encInfo->extn_secret_file, encInfo) == e_success)
                        {
                            printf("Encoded secret file extension successfully\n");
                            if (encode_secret_file_size(encInfo->size_secret_file, encInfo) == e_success)
                            {
                                printf("Encoded secret file size successfully\n");
                                if (encode_secret_file_data(encInfo) == e_success)
                                {
                                    printf("Encoded secret data successfully\n");
                                    if (copy_remaining_img_data(encInfo->fptr_src_image, encInfo->fptr_stego_image) == e_success)
                                    {
                                        printf("copied remaining bytes successfully\n");
                                    }
                                    else
                                    {
                                        printf("Failed to copy  remaining data\n");
                                    }
                                }
                                else
                                {
                                    printf("Failed to encode secret data\n");
                                }
                            }
                            else
                            {
                                printf("Failed to encode secret file size\n");
                                return e_failure;
                            }
                        }
                        else
                        {
                            printf("Failed to encode secret file extension\n");
                            return e_failure;
                        }
                    }
                    else
                    {
                        printf("Failed to encode secret file extension size\n");
                        return e_failure;
                    }
                }
                else
                {
                    printf("Failed to encode the magic string\n");
                }
            }
            else
            {
                printf("Failed to copy header\n");
                return e_failure;
            }
        }
        else
        {
            printf("image does not have suffecient size for encoding\n");
            return e_failure;
        }
        return e_success;
    }
    else
    {
        printf("Failed to open files\n");
        return e_failure;
    }
    return e_success;
}
