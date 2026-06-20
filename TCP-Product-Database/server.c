#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 8080
#define BUFFER_SIZE 1024
#define DB_FILE "products.txt"

typedef struct
{
    char name[50];
} Product;

pthread_mutex_t db_mutex = PTHREAD_MUTEX_INITIALIZER;

void handlePost(int client, char *name)
{
    Product p;
    memset(&p, 0, sizeof(Product));
    strncpy(p.name, name, sizeof(p.name) - 1);

    pthread_mutex_lock(&db_mutex);

    FILE *fp = fopen(DB_FILE, "ab");
    fseek(fp, 0, SEEK_END);
    fwrite(&p, sizeof(Product), 1, fp);
    fclose(fp);

    pthread_mutex_unlock(&db_mutex);

    send(client, "Product successfully added\n", 27, 0);
}

void handleGetOne(int client, char *name)
{
    Product p;
    int found = 0;

    pthread_mutex_lock(&db_mutex);

    FILE *fp = fopen(DB_FILE, "rb");

    while (fread(&p, sizeof(Product), 1, fp) == 1)
    {
        if (strcmp(p.name, name) == 0)
        {
            send(client, p.name, strlen(p.name), 0);
            send(client, "\n", 1, 0);
            found = 1;
            break;
        }
    }

    fclose(fp);

    pthread_mutex_unlock(&db_mutex);

    if (!found)
    {
        send(client, "Not Found\n", 10, 0);
    }
}

void handleGetAll(int client)
{
    Product p;
    char response[4096] = "";

    pthread_mutex_lock(&db_mutex);

    FILE *fp = fopen(DB_FILE, "rb");

    while (fread(&p, sizeof(Product), 1, fp) == 1)
    {
        strcat(response, p.name);
        strcat(response, "\n");
    }

    fclose(fp);

    pthread_mutex_unlock(&db_mutex);

    if (strlen(response) == 0)
    {
        strcpy(response, "Catalog Empty\n");
    }

    send(client, response, strlen(response), 0);
}

void *client_thread(void *arg)
{
    int client = *(int *)arg;
    free(arg);

    char buffer[BUFFER_SIZE];

    int bytes = recv(client, buffer, sizeof(buffer) - 1, 0);

    if (bytes > 0)
    {
        buffer[bytes] = '\0';

         char name[50];

        if (strncmp(buffer, "POST ", 5) == 0)
        {
            sscanf(buffer, "POST %49[^\n]", name);
            handlePost(client, name);
        }
        else if (strcmp(buffer, "GET") == 0 ||
                 strcmp(buffer, "GET\n") == 0)
        {
            handleGetAll(client);
        }
        else if (strncmp(buffer, "GET ", 4) == 0)
        {
            sscanf(buffer, "GET %49[^\n]", name);
            handleGetOne(client, name);
        }
        else
        {
            send(client, "Invalid Command\n", 16, 0);
        }
    }

    close(client);
    return NULL;
}

int main()
{
    FILE *fp = fopen(DB_FILE, "ab");
    fclose(fp);

    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in serverAddr;

    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(PORT);
    serverAddr.sin_addr.s_addr = INADDR_ANY;

    bind(serverSocket,
         (struct sockaddr *)&serverAddr,
         sizeof(serverAddr));

    listen(serverSocket, 10);

    printf("Server listening on %d\n", PORT);

    while (1)
    {
        int *client = (int*)malloc(sizeof(int));

        *client = accept(serverSocket, NULL, NULL);

        pthread_t tid;

        pthread_create(&tid,
                       NULL,
                       client_thread,
                       client);

        pthread_detach(tid);
    }

    close(serverSocket);

    return 0;
}