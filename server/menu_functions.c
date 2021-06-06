#include "menu_functions.h"
#include "db.h"
#include "helpers.h"
#include <stdio.h>
#include <stdlib.h>

void add_song_collection()
{
    // TODO: Validate input
    Song new_song;
    get_line("Title: ", new_song.title, sizeof new_song.title);
    get_line("Artist: ", new_song.artist, sizeof new_song.artist);
    printf("Length: ");
    scanf("%f", &new_song.length);
    db_add_song(new_song, MUSIC_TABLE);
}

void download_missing_songs()
{
    // TODO
    db_get_missing_songs();
}

void show_missing_songs()
{
    printf("------------------ MISSING SONGS ------------------\n");
    db_execute("SELECT * FROM " MISSING_TABLE);
    printf("---------------------------------------------------\n");
}

void show_all_songs()
{
    printf("------------------ ALL SONGS ------------------\n");
    db_execute("SELECT * FROM " MUSIC_TABLE);
    printf("-----------------------------------------------\n");
}

void show_query()
{
    printf("------------------ QUERY ------------------\n");
    db_execute("SELECT * FROM " QUERY_TABLE);
    printf("-------------------------------------------\n");
}