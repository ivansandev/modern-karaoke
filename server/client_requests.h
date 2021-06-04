#ifndef CLIENT_MODULE
#define CLIENT_MODULE

#include <stdio.h>
#include <stdlib.h>
#include "db.h"

int request_song(struct Song song)
{
    // Returns: int
    //          <0 -> error requesting song
    //           0 -> Song found and requested
    //           1 -> Song not found

    int song_found = db_find_song(song, MUSIC_TABLE);
    if (song_found < 0) {
        printf("Error requesting song.\n");
        return -1;
    }
    else if (song_found == 0)
    {
        // add_missing_song(song);
        db_add_song(song, MISSING_TABLE);
        return 1;
    }
    else {
        // TODO: Check if song is queried and notify client
        db_add_song(song, QUERY_TABLE);
        return 0;
    }
}

void now_playing(char *buffer);

#endif /* CLIENT_MODULE */