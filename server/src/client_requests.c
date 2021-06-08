#include "client_requests.h"
#include <stdio.h>
#include <stdlib.h>

int request_song(struct Song song)
{
    // Returns: int
    //        <=-1 -> error requesting song
    //           0 -> Song found and requested
    //           1 -> Song not found, added in request_list for future parties

    int song_id = db_find_id_song(song);
    if (song_id < 0) {
        printf("Error requesting song.\n");
        return -1;
    }
    else if (song_id == 0)
    {
        db_add_song(song, MISSING_TABLE);
        return 1;
    }
    else {
        db_query_song(song_id);
        return 0;
    }
}
