
/*
Name:
Date:
Description:
Sample input:
Sample Output:
*/
#include <stdlib.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <string.h>
#include <sys/types.h>
#include <error.h>
#include <unistd.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

#define CLIENT_BUFF_SIZE 1024

// #define FILE_PATH "/home/emertxe/"
int main(int argc, char *argv[])
{
    int c_size;
    if (argc != 3)
    {
        printf("Usage: ./a.out <server ip addr> <server port no.>\n");
        return -1;
    }

    int sock_fd, file_fd;
    char client_buffer[CLIENT_BUFF_SIZE];
    char file_name[50];
    struct sockaddr_in server_addr;

    // creating the socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("Error: could not create socket\n");
        return -1;
    }

    // ADDING SERVER INFO
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2]));
    server_addr.sin_addr.s_addr = inet_addr(argv[1]);

    printf("Enter the file name here: \n");
    scanf("%s", file_name);

    //  snprintf(file_name,sizeof(file_name),"%s%s",FILE_PATH,client_buffer);
    printf("filename is %s\n", file_name); // cheking purpose only

    if (connect(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("connect");
        return -1;
    }
    c_size = send(sock_fd, file_name, strlen(file_name), 0);
    if (c_size > 0)
    {
        printf("request sent successfully\n");
    }

    ssize_t bytes;
    file_fd = open(file_name, O_WRONLY | O_CREAT | O_TRUNC, 0666);
    if (file_fd < 0)
    {
        perror("Error opening file");
        close(sock_fd);
        return -1;
    }
    const char *msg = "File not found";
    while ((bytes = recv(sock_fd, client_buffer, sizeof(client_buffer), 0)) > 0)
    {
        if (strcmp(msg, client_buffer) == 0)
        {
            unlink(file_name);
            write(1, client_buffer, bytes);
            break;
        }

        if (write(file_fd, client_buffer, bytes) != 0)
        {
            perror("write");
            close(sock_fd);
            close(file_fd);
            return 0;
        }
    }
    if (bytes == 0)
    {
        printf("File transferred successful\n");
    }
    else
    {
        printf("file transferred failed\n");
    }

    close(sock_fd);
    close(file_fd);

    return 0;
}
