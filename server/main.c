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
#include "helper_functions.h"
#include "client_requests.h"

#define PORT 8080
#define NET_BUF_SIZE 128
#define ADDR "127.0.0.1"
#define MAX_CLIENTS 100
#define MAX_CLIENT_MSG 1000

#define MUSIC_TABLE "song_collection"
#define MISSING_TABLE "missing_songs"
#define QUERY_TABLE "query_playlist"

#define ERROR_MSG "ERROR"
#define SONG_UNAVAILABLE_MSG "SNGNA"
#define SONG_REQUESTED_MSG "SNGOK"

int PARTY_STARTED = 0;
// pthread_cond_t PARTY_STARTED = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER;

// typedef struct Song
// {
//     char title[100];
//     char artist[100];
//     float length;
// } Song;

void clearBuffer(char *buf, int charlen)
{
    int i;
    for (i = 0; i < charlen; i++)
        buf[i] = '\0';
}

void sig_handler(int signum)
{

}

void *clientThread(void *arg)
{
    char msg[MAX_CLIENT_MSG];
    int sockfd = *((int *)arg);

    Song song_request;

    // message request
    // TODO: Check request type
    //       - Song request
    //       - Now playing
    //       - Disconnect

    recv(sockfd, msg, MAX_CLIENT_MSG, 0);
    printf("MESSAGE: %s\n", msg);
    if (strcmp(msg, "REQUEST") == 0)
    {
        clearBuffer(msg, MAX_CLIENT_MSG);
        // REQUEST: Song request
        // SONG NAME
        recv(sockfd, msg, MAX_CLIENT_MSG, 0);
        strcpy(song_request.title, msg);
        clearBuffer(msg, MAX_CLIENT_MSG);

        // SONG AUTHOR
        recv(sockfd, msg, MAX_CLIENT_MSG, 0);
        strcpy(song_request.artist, msg);
        clearBuffer(msg, MAX_CLIENT_MSG);

        printf("Requesting song '%s' by '%s'\n", song_request.title, song_request.artist);
        int requested = request_song(song_request);
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
        printf("1 -> DBG: Weird message %s", msg);
    }
    else if (strcmp(msg, "DISCONNECT") == 0)
    {
        // REQUEST: Disconnect
        printf("2 -> DBG: Weird message %s", msg);
    }
    else
    {
        printf("DBG: Weird message %s", msg);
    }

    printf("DBG: Thread finished.\n");

    return 0;
}

void startParty(int sockfd, struct sockaddr_in *server_addr, char *net_buf)
{
    // Bind the address struct to the socket
    if (bind(sockfd, (struct sockaddr *)server_addr, sizeof(*server_addr)) != 0)
        printf("\nError occured while binding socket.\n");

    // Listen for upcoming requests and start thread with socket connection for every client
    printf("\nWaiting for guests...\n");
    if (listen(sockfd, MAX_CLIENTS) != 0)
    {
        perror("\nCannot listen");
        return;
    }

    pthread_t clients_con[MAX_CLIENTS];
    int i = 0;
    while (1)
    {
        // accept call is creating a new socket for upcoming connection
        socklen_t addr_size = sizeof server_addr;
        int clientSocket = accept(sockfd, (struct sockaddr *)&server_addr, &addr_size);

        if (pthread_create(&clients_con[i++], NULL, clientThread, &clientSocket) != 0)
            printf("\nCannot create socket for client\n");
    }
}

void downloadMissingSongs()
{
    printf("\nSongs downloaded!\n");
}

void player()
{
    // thread, takes care of whats now playing
    // * plays music from QUERY_LIST; Every "song_length" minutes, it deletes the song from the query
    // * if QUERY_LIST is empty, gets random song and adds it to the QUERY_LIST
}

int main()
{
    // TESTING DB -----------------------------------------------------------------
    // initialize_db_tables();
    // Song new_song = {
    //     .title = "Animals",
    //     .artist = "Martin Garrix",
    //     .length = 3.5
    // };
    // db_add_song(new_song, MUSIC_TABLE);

    // Song song_request = {
    //     .title = "workaholic",
    //     .artist = "ambulance"
    // };

    // printf("Song '%s - %s' found: %d\n", song_request.artist, song_request.title, request_song(song_request));

    // return 0;
    // ----------------------------------------------------------------------------

    // SOCKET PREREQUISITES / CONFIGURATION
    // -------------------------------------------------------
    initialize_db_tables();

    int sockfd;
    // sockfd = socket(AF_INET, SOCK_DGRAM, 0); // SOCK_DGRAM doesn't support listen()
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd == 0)
        perror("\nCannot create socket for listening.\n");

    struct sockaddr_in server_addr;
    struct sockaddr_storage server_storage;

    server_addr.sin_family = AF_INET;              // internet
    server_addr.sin_port = htons(PORT);            // port
    server_addr.sin_addr.s_addr = inet_addr(ADDR); // INADDR_ANY
    // server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(server_addr.sin_zero, '\0', sizeof server_addr.sin_zero);

    char net_buf[NET_BUF_SIZE];
    int msg_bytes;

    // -------------------------------------------------------

    // MAIN MENU
    // -------------------------------------------------------
    while (1)
    {
        if (PARTY_STARTED)
        {
            printf("\t1.End party\n");
        }
        else
        {
            printf("\t1. Start party\n");
            printf("\t2. Download new song\n");
            printf("\t3. Download requested missing songs\n");
        }
        printf("Choice: ");
        short choice;
        scanf("%hd", &choice);
        while ((getchar()) != '\n')
            ;

        Song new_song;

        switch (choice)
        {
        case 1:
            // Start listening on PORT when party starts (TODO: in a new process)
            printf("Party started!");
            startParty(sockfd, &server_addr, net_buf);
            break;
        case 2:
            // Add new song logic;
            getLine("Title: ", new_song.title, sizeof new_song.title);
            getLine("Artist: ", new_song.artist, sizeof new_song.artist);
            printf("Length: ");
            scanf("%f", &new_song.length);
            db_add_song(new_song, MUSIC_TABLE);
            break;
        case 3:
            // Download missing songs to DB
            downloadMissingSongs();
            break;
        default:
            printf("\nWrong choice!\n");
        }
    }
    // -------------------------------------------------------

    return 0;
}

void download_missing_songs()
{
    // Get all data from MISSING_TABLE
    // Insert all data to MUSIC_COLLECTION
}