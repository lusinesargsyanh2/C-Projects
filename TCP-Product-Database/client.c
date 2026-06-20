#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/time.h>

#define PORT 8080
#define BUFFER_SIZE 4096

int main()
{
    char command[1024];

    printf("Enter command: ");
    fgets(command, sizeof(command), stdin);

    command[strcspn(command, "\n")] = '\0';

    int sock = socket(AF_INET, SOCK_STREAM, 0);

    struct timeval tv = {5, 0};

    setsockopt(sock,
               SOL_SOCKET,
               SO_RCVTIMEO,
               &tv,
               sizeof(tv));

    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    inet_pton(AF_INET, "127.0.0.1", &serverAddr.sin_addr);

    if (connect(sock,
                (struct sockaddr *)&serverAddr,
                sizeof(serverAddr)) < 0)
    {
        perror("connect");
        return 1;
    }

    send(sock, command, strlen(command), 0);

    char buffer[BUFFER_SIZE];

    int bytes = recv(sock, buffer, sizeof(buffer) - 1, 0);

    if (bytes > 0)
    {
        buffer[bytes] = '\0';
        printf("%s\n", buffer);
    }
    else
    {
        printf("No response or timeout\n");
    }

    close(sock);

    return 0;
}