#include "menu_functions.h"
#include "../db/db.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>

void add_song_wizard()
{
    // TODO: Validate input
    Song new_song;
    get_line("Title: ", new_song.title, sizeof new_song.title);
    get_line("Artist: ", new_song.artist, sizeof new_song.artist);
    get_line("Genre: ", new_song.genre, sizeof new_song.genre);
    db_add_song(new_song, MUSIC_TABLE);
}

void download_missing_songs()
{
    db_download_missing_songs();
}

void show_missing_songs()
{
    printf("------------------ MISSING SONGS ------------------\n");
    db_execute("SELECT * FROM " MISSING_TABLE);
    printf("---------------------------------------------------\n");
}

void show_songs()
{
    printf("------------------ ALL SONGS ------------------\n");
    db_execute("SELECT * FROM " MUSIC_TABLE);
    printf("-----------------------------------------------\n");
}

void show_query()
{
    printf("------------------ QUERY ------------------\n");
    db_execute("SELECT title, artist, requested_at FROM " QUERY_TABLE " LEFT JOIN " MUSIC_TABLE " ON " QUERY_TABLE ".song_id = " MUSIC_TABLE ".rowid;");
    printf("-------------------------------------------\n");
}

void play_next_request()
{
    /*
        - Check if QUERY is empty
        - Delete latest song from query
        - ...
    */
}