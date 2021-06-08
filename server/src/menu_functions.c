#include "menu_functions.h"
#include "db.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>

void add_song_wizard()
{
    // TODO: Validate input
    Song new_song = { 0 };
    getchar();
    get_line("Title: ", new_song.title, sizeof new_song.title);
    get_line("Artist: ", new_song.artist, sizeof new_song.artist);
    db_add_song(new_song, MUSIC_TABLE);
}

void download_missing_songs()
{
    db_download_missing_songs();
}

void show_missing_songs()
{
    printf("\n------------------ MISSING SONGS ------------------\n");
    db_execute("SELECT title, artist FROM " MISSING_TABLE);
    printf("---------------------------------------------------\n\n");
}

void show_songs()
{
    printf("\n------------------ ALL SONGS ------------------\n");
    db_execute("SELECT artist, title, created_at FROM " MUSIC_TABLE " ORDER BY artist");
    printf("-----------------------------------------------\n\n");
}

void show_query()
{
    printf("\n------------------ QUERY ------------------\n");
    db_execute("SELECT title, artist, requested_at FROM " QUERY_TABLE " LEFT JOIN " MUSIC_TABLE " ON " QUERY_TABLE ".song_id = " MUSIC_TABLE ".rowid "
               "ORDER BY requested_at");
    printf("-------------------------------------------\n\n");
}

void play_next_request()
{
    int next_song_id = db_is_query_empty();
    if (next_song_id < -1)
    {
        printf("Error occured while playing next song.\n");
        return;
    }
    else if (next_song_id == -1) {
        printf("--------------------------------\n");
        printf("--> There are no requests, query is empty!\n");
        printf("--------------------------------\n");
        return;
    }
    else {
        printf("+------------------------------------------+\n");
        printf("  NEXT SONG:\n    ");
        db_execute("SELECT title, artist FROM " QUERY_TABLE " LEFT JOIN " MUSIC_TABLE " ON " QUERY_TABLE ".song_id = " MUSIC_TABLE ".rowid ORDER BY requested_at LIMIT 1");
        printf("+------------------------------------------+\n");
        db_execute("DELETE FROM " QUERY_TABLE " WHERE rowid IN (SELECT rowid FROM " QUERY_TABLE " ORDER BY requested_at ASC LIMIT 1)");
    }
}