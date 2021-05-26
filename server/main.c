#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#include <time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#define PORT 8080
#define NET_BUF_SIZE 128
#define ADDR "127.0.0.1"
#define MAX_CLIENTS 100

void clearBuffer(char *buf)
{
    int i;
    for (i = 0; i < NET_BUF_SIZE; i++)
        buf[i] = '\0';
}

void clientThread() {}

void listenForClients(struct sockaddr_in *addr_con, char *net_buf)
{
    if (listen(addr_con, MAX_CLIENTS) == 0)
    {
        printf("\nListening!\n");
    }
    else
    {
        perror("\nCannot listen");
        return;
    }

    pthread_t clients_con[MAX_CLIENTS];
    int i = 0;
    while (1)
    {
        // accept call is creating a new socket for upcoming connection
        socklen_t addr_size = sizeof addr_con;
        int newSocket = accept(addr_con, (struct sockaddr *)&addr_con, &addr_size);
        if (pthread_create(clients_con[i++], NULL, clientThread, &newSocket) != 0)
            printf("\nCannot create socket for client\n");

        if (i >= MAX_CLIENTS)
        {
            i = 0;
            while (i < 50)
            {
                pthread_join(clients_con[i++], NULL);
            }
            i = 0;
        }
    }
}

void startParty(int sockfd, struct sockaddr_in *addr_con, char *net_buf)
{
    // Listen for upcoming requests and start thread with socket connection for every client
    printf("\nWaiting for guests...\n");

    if (bind(sockfd, (struct sockaddr *)addr_con, sizeof(addr_con)) == 0)
        printf("\nParty started.\n");
    else
        printf("\nError occured while starting party.\n");

    // create new thread for listening
    listenForClients(addr_con, net_buf);
}

void downloadMissingSongs()
{
    printf("\nSongs downloaded!\n");
}

int main()
{
    int sockfd;
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sockfd < 0)
        perror("\nCannot create socket for listening.\n");

    struct sockaddr_in addr_con;
    addr_con.sin_family = AF_INET;
    addr_con.sin_port = htons(PORT);
    addr_con.sin_addr.s_addr = inet_addr(ADDR); // INADDR_ANY
    memset(addr_con.sin_zero, '\0', sizeof addr_con.sin_zero);

    char net_buf[NET_BUF_SIZE];
    int bytes_transfer;

    printf("\t1. Start party\n");
    printf("\t2. Download missing songs\n");
    printf("Choice: ");
    short choice;
    scanf("%hd", &choice);
    switch (choice)
    {
    case 1:
        // Start listening on PORT when party starts
        printf("Party started!");
        // pthread_t party_thread;
        // pthread_create(&party_thread, NULL, startParty, addr_con, net_buf);
        startParty(sockfd, &addr_con, net_buf);
        break;
    case 2:
        // Download missing songs to DB
        downloadMissingSongs();
        break;
    default:
        printf("\nWrong choice!\n");
    }
}