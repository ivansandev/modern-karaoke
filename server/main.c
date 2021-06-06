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
#include "db.h"
#include "helpers.h"
#include "client_requests.h"
#include "player.h"
#include "menu_functions.h"

#define ADDR "127.0.0.1"
#define PORT 8080
#define MAX_CLIENTS 100
#define PACKAGE_LEN 1024

// RESPONSES FOR CLIENT'S REQUEST
#define ERROR_MSG "ERROR"
#define SONG_UNAVAILABLE_MSG "SNGNA"
#define SONG_REQUESTED_MSG "SNGOK"


int PARTY_STARTED = 0;
// pthread_cond_t PARTY_STARTED = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;
int sockfd = 0;
pthread_t clients_con[MAX_CLIENTS], player_thread, party_thread;


void clearBuffer(char *buf, int charlen)
{
    int i;
    for (i = 0; i < charlen; i++)
        buf[i] = '\0';
}

void handle_sigint(int signum)
{
    /*
    Takes care when the server stops, to kill all threads and close all client socket conections.
    */
    printf("\nParty ending!\n");
    PARTY_STARTED = 0;
    int i=0;
    for (i=0; i< MAX_CLIENTS; i++)
    {
        pthread_join(clients_con[i], NULL);
    }
    close(sockfd);
    exit(0);
}

void *clientThread(void *arg)
{
    char msg[PACKAGE_LEN];
    int sockfd = *((int *)arg);

    // Client's initial message request can be:
    //       - Song request
    //       - Now playing
    //       - Disconnect

    // Receive what kind of request does the client have
    printf("-> DEBUG: Receive request type\n");
    if (recv(sockfd, msg, PACKAGE_LEN, 0) == 0)
    {
        // Close connection
        close(sockfd);
        return 0;
    }

    send(sockfd, "OK", 2, 0);

    // SONG REQUEST
    if (strcmp(msg, "REQUEST") == 0)
    {
        Song song_request;
        // RECEIVE SONG TITLE
        clearBuffer(msg, PACKAGE_LEN);
        printf("-> DEBUG: Receive song title\n");
        recv(sockfd, msg, PACKAGE_LEN, 0);
        strcpy(song_request.title, msg);
        clearBuffer(msg, PACKAGE_LEN);
        send(sockfd, "OK", 2, 0);

        // RECEIVE SONG ARTIST
        printf("-> DEBUG: Receive song artist\n");
        recv(sockfd, msg, PACKAGE_LEN, 0);
        strcpy(song_request.artist, msg);
        clearBuffer(msg, PACKAGE_LEN);
        send(sockfd, "OK", 2, 0);

        printf("Song requested: %s - %s\n", song_request.title, song_request.artist);
        pthread_mutex_lock(&mtx);
        int requested = request_song(song_request);
        pthread_mutex_unlock(&mtx);
        if (requested < -1)
        {
            // send: Error
            send(sockfd, ERROR_MSG, 5, 0);
        }
        else if (requested == 0)
        {
            // send: Song requested
            send(sockfd, SONG_REQUESTED_MSG, 5, 0);
        }
        else if (requested == 1)
        {
            // send: Song not available
            send(sockfd, SONG_UNAVAILABLE_MSG, 5, 0);
        }
    }
    else if (strcmp(msg, "NOW") == 0)
    {
        // REQUEST: Now playing
        // TODO: Get latest song from QUERY_TABLE
    }
    else
    {
        printf("-> DEBUG: Unknown requrest %s\n", msg);
    }

    printf("-> DEBUG: Closing socket, ending thread.\n");
    close(sockfd);

    return 0;
}

// void *start_party(struct sockaddr_in *server_addr)
void *start_party(void *addr)
{
    signal(SIGINT, handle_sigint);
    struct sockaddr_in *server_addr = ((struct sockaddr_in*) addr);
    // Bind the address struct to the socket
    if (bind(sockfd, (struct sockaddr *)server_addr, sizeof(*server_addr)) != 0)
        printf("\nError occured while binding socket.\n");

    // Listen for upcoming requests and start thread with socket connection for every client
    printf("\nWaiting for guests...\n");
    if (listen(sockfd, MAX_CLIENTS) != 0)
    {
        perror("\nCannot listen");
        return NULL;
    }

    int i = 0;
    while (1)
    {
        // TODO: Decrease number of connections when a client disconnects and vice versa
        // TODO: Make clients_con into a list, so that deleting / appending can be optimized

        // Create socket + thread for every connection
        socklen_t addr_size = sizeof server_addr;
        int clientSocket = accept(sockfd, (struct sockaddr *)&server_addr, &addr_size);

        if (pthread_create(&clients_con[i++], NULL, clientThread, &clientSocket) != 0)
            printf("\nCannot create socket for client\n");
    }
}

int main()
{
    // PREREQUISITES / CONFIGURATION
    // -------------------------------------------------------
    initialize_db();

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == 0)
        perror("\nCannot create socket for listening.\n");

    struct sockaddr_in server_addr;
    // struct sockaddr_storage server_storage;

    server_addr.sin_family = AF_INET;              // internet
    server_addr.sin_port = htons(PORT);            // port
    server_addr.sin_addr.s_addr = inet_addr(ADDR); // INADDR_ANY
    memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);
    setsockopt(sockfd,SOL_SOCKET,SO_REUSEADDR,NULL,sizeof(int));

    // -------------------------------------------------------
    // MAIN MENU
    // -------------------------------------------------------
    short menu_choice;
    while (1)
    {
        if (PARTY_STARTED == 1)
        {
            printf("\t1.End party\n");
            printf("\t2. Show requested songs\n");

            printf("Choice: ");
            scanf("%hd", &menu_choice);
            while ((getchar()) != '\n');

            switch (menu_choice)
            {
                case 1:
                    pthread_join(party_thread, 0);
                    pthread_join(player_thread, 0);
                    PARTY_STARTED = 0;
                    break;
                case 2:
                    show_query();
                    break;
                default:
                    printf("Invalid choice.\n");
            }
        }
        else
        {
            printf("\t1. Start party\n");
            printf("\t2. Add new song\n");
            printf("\t3. Add requested missing songs\n");
            printf("\t4. Show missing songs\n");
            printf("\t5. Show all songs\n");

            printf("Choice: ");
            scanf("%hd", &menu_choice);
            while ((getchar()) != '\n');

            switch (menu_choice)
            {
            case 1:
                // TODO: Create process/thread for party
                // TODO: Create process/thread for player
                PARTY_STARTED = 1;
                pthread_create(&player_thread, NULL, start_player, NULL);
                pthread_create(&party_thread, NULL, start_party, &server_addr);
                printf("Party started!\n");
                break;
            case 2:
                add_song_collection();
                break;
            case 3:
                download_missing_songs();
                break;
            case 4:
                show_missing_songs();
                break;
            case 5:
                show_all_songs();
                break;
            default:
                printf("\nUnknown choice!\n");
            }
        }
    }
    // -------------------------------------------------------

    return 0;
}
