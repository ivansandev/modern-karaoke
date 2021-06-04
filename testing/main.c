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

int main()
{
    pthread_t thread[100];
    if (thread[1] == NULL)
        printf("Test\n");
    else
        printf("Not null\n");

    printf("%s\n", thread[1]);

    return 0;
}