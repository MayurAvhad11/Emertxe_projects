
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
#include <unistd.h>
#include <error.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>

#define BUFF_SIZE 1024
#define FILE_PATH "/home/emertxe/"
#define PORT_NUM 2020
int main()
{
    int sock_fd, data_sock_fd, buffer_len;
    struct sockaddr_in server_addr;
    int addr_len = sizeof(server_addr);
    char server_buffer[BUFF_SIZE] = {0}; // initialize it for avoiding GV
    char buffer[BUFF_SIZE] = {0};

    printf("server is running\n");

    // creating the socket
    if ((sock_fd = socket(AF_INET, SOCK_STREAM, 0)) == -1)
    {
        perror("socket");
        return -1;
    }

    // BINDDING
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY; // allow any ip to connect
    server_addr.sin_port = htons(PORT_NUM);

    if (bind(sock_fd, (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1)
    {
        perror("bind");
        return -1;
    }
    while (1)
    {
        if (listen(sock_fd, 5) < 0)
        {
            perror("listne");
            return -1;
        }

        if ((data_sock_fd = accept(sock_fd, (struct sockaddr *)&server_addr, (socklen_t *)&addr_len)) < 0)
        {
            perror("accept");
            return -1;
        }

        recv(data_sock_fd, server_buffer, BUFF_SIZE, 0);
        // read(data_sock_fd, server_buffer, BUFF_SIZE);
        printf("The requested file is %s\n", server_buffer);
        char file_name[BUFF_SIZE];
        snprintf(file_name, sizeof(file_name), "%s%s", FILE_PATH, server_buffer);

        int file_fd;
        file_fd = open(file_name, O_RDONLY);
        if (file_fd == -1)
        {
            const char *msg = "File not found";
            send(data_sock_fd, msg, strlen(msg), 0);
        }
        else
        {
            ssize_t read_bytes;
            while ((read_bytes = read(file_fd, buffer, BUFF_SIZE)) > 0)
            {
                send(data_sock_fd, buffer, read_bytes, 0);
            }
            close(file_fd);
            printf("File sent successfully.\n");
        }
    }
    close(data_sock_fd);
    close(sock_fd);
    return 0;
}
