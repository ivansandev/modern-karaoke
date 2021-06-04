#include <stdio.h>
#include <stdlib.h>
#include "player.h"

int start_player()
{
    // thread, takes care of whats now playing
    // * plays music from QUERY_LIST; Every "song_length" minutes, it deletes the song from the query
    // * if QUERY_LIST is empty, gets random song and adds it to the QUERY_LIST
}