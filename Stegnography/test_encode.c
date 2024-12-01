/*
Name:Mayur Anil Avhad
Date:12/05/2024
Description:Project_1: Steganography
Sample input:./a.out -e beautiful.bmp secret.txt stego.bmp
Sample Output:
Selected Encoding
Read and validate encode argumet is successful
Successfully opend all requried files
width = 1024
height = 768
Encoding is possible
copied header successfully
successfully encoded magic string
Successfully encoded file extension size successfully
Encoded secret file extension successfully
Encoded secret file size successfully
Encoded secret data successfully
copied remaining bytes successfully
Completed Encoding


*/

#include <stdio.h>
#include <string.h>
#include "encode.h"
#include "decode.h"
#include "types.h"

int main(int argc, char *argv[])
{
    if (check_operation_type(argv) == e_encode) // if user selecte -e
    {
        EncodeInfo encInfo;
        printf("Selected Encoding\n");
        if (read_and_validate_encode_args(argv, &encInfo) == e_success) // validating the user arguments
        {
            printf("Read and validate encode argumet is successful\n");
            if (do_encoding(&encInfo) == e_success) // Encoding
            {
                printf("Completed Encoding\n");
            }
            else
            {
                printf("Encoding failed\n");
            }
        }
        else
        {
            printf("Failed: read and validate encode argumets\n");
        }
    }
    else if (check_operation_type(argv) == e_decode) // if user selecte -d
    {
        DecodeInfo decInfo;
        printf("Selected Decoding\n");
        if (read_and_validate_decode_args(argv, &decInfo) == d_success) // validating the user arguments
        {
            printf("Read and validate decode argument is successful\n");
            if (do_decoding(&decInfo) == d_success) // Decoding
            {
                printf("Completed Decoding\n");
            }
            else
            {
                printf("Deconding failed\n");
            }
        }
        else
        {
            printf("Failed to validate decode arguments\n");
        }
    }
    else
    {
        printf("Invalid Option\n***usage***\n");
        printf("Encoding: ./a.out -e beautiful.bmp secret.txt stego.bmp\n");
        printf("Decoding: ./a.out -e beautiful.bmp output.txt\n");
    }

    return 0;
}
OperationType check_operation_type(char *argv[]) // check user enter -e or -d
{
    if (strcmp(argv[1], "-e") == 0)
        return e_encode;
    else if (strcmp(argv[1], "-d") == 0)
        return e_decode;
    else
        return e_unsupported;
}
