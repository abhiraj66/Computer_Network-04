#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void find_student(char *reg_no, char *response)
{
    FILE *fp = fopen("students.txt", "r");

    if (fp == NULL)
    {
        strcpy(response, "Error opening file");
        return;
    }

    char file_reg[50];
    char name[50];
    char branch[50];
    char college[100];

    int found = 0;

    while (fscanf(fp, "%s %s %s %s",
                  file_reg, name, branch, college) != EOF)
    {
        if (strcmp(file_reg, reg_no) == 0)
        {
            sprintf(response,
                    "Name: %s, Branch: %s, College: %s",
                    name, branch, college);

            found = 1;
            break;
        }
    }

    if (!found)
    {
        strcpy(response, "Data not found");
    }

    fclose(fp);
}

int main()
{
    int server_fd, new_socket;
    struct sockaddr_in address;

    int addrlen = sizeof(address);

    char buffer[BUFFER_SIZE];
    char response[BUFFER_SIZE];

    server_fd = socket(AF_INET, SOCK_STREAM, 0);

    if (server_fd < 0)
    {
        perror("Socket failed");
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd,
             (struct sockaddr *)&address,
             sizeof(address)) < 0)
    {
        perror("Bind failed");
        return 1;
    }

    if (listen(server_fd, 5) < 0)
    {
        perror("Listen failed");
        return 1;
    }

    printf("Server waiting for connection...\n");

    new_socket = accept(server_fd,
                        (struct sockaddr *)&address,
                        (socklen_t *)&addrlen);

    if (new_socket < 0)
    {
        perror("Accept failed");
        return 1;
    }

    printf("Client connected.\n");

    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);

        int bytes = read(new_socket,
                         buffer,
                         BUFFER_SIZE - 1);

        if (bytes <= 0)
        {
            break;
        }

        buffer[bytes] = '\0';

        if (strcmp(buffer, "bye") == 0)
        {
            printf("Client requested termination.\n");
            break;
        }

        memset(response, 0, BUFFER_SIZE);

        find_student(buffer, response);

        send(new_socket,
             response,
             strlen(response),
             0);
    }

    close(new_socket);
    close(server_fd);

    printf("Server closed.\n");

    return 0;
}
