#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>

#include "db.h"
#include "../src/helpers.h"

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for (i = 0; i < argc; i++)
    {
        //printf("%s = %s \t ", azColName[i], argv[i] ? argv[i] : "NULL");
        printf("%s  |  ", argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int db_execute(char *query)
{
    sqlite3 *db;
    int rc = sqlite3_open(DB_FILENAME, &db);
    if (rc)
    {
        fprintf(stderr, "Cannot open DB: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    char *err; // SQL execution error message
    rc = sqlite3_exec(db, query, callback, 0, &err);

    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "SQL execution error: %s\n", err);
        sqlite3_free(err);
        sqlite3_close(db);
        return 1;
    }

    sqlite3_close(db);
    return 0;
}

int db_create_table_music()
{
    char *query;
    query = "CREATE TABLE IF NOT EXISTS " MUSIC_TABLE
            "("
            "title          TEXT    NOT NULL,"
            "artist         TEXT    NOT NULL,"
            "genre          TEXT,"
            "created_at     TEXT    NOT NULL DEFAULT CURRENT_TIMESTAMP,"
            "UNIQUE(title, artist))";
    int status = db_execute(query);
    if (status > 0)
        fprintf(stderr, "Error while creating 'music_collection' table. \n");
    // else if (status == 0)
    //     printf("'music_collection' table created!\n");
    return status;
}

int db_create_table_query()
{
    char *query;
    query = "CREATE TABLE IF NOT EXISTS " QUERY_TABLE
            "("
                "song_id         INT     NOT NULL,"
                "requested_at    TEXT    NOT NULL DEFAULT CURRENT_TIMESTAMP,"
                "UNIQUE(song_id)"
            ")";
    int status = db_execute(query);
    if (status > 0)
        fprintf(stderr, "Error while creating 'query' table.\n");
    return status;
}

int db_create_table_missing_songs()
{
    char *query;
    query = "CREATE TABLE IF NOT EXISTS " MISSING_TABLE
            "("
                "title           TEXT    NOT NULL,"
                "artist          TEXT    NOT NULL,"
                "requested_at    TEXT    NOT NULL DEFAULT CURRENT_TIMESTAMP"
                // "UNIQUE(title, artist)"
            ")";
    int status = db_execute(query);
    if (status > 0)
        fprintf(stderr, "Error while creating 'missing_songs' table.\n");
    return status;
}

void initialize_db()
{
    db_create_table_music();
    db_create_table_query();
    db_create_table_missing_songs();
    db_execute("DELETE FROM " QUERY_TABLE);
}

int db_find_id_song(struct Song song)
{
    // Returns: int
    //          <0 -> error
    //           0 -> not found
    //           1 -> found
    sqlite3 *db;
    sqlite3_stmt *res;
    char query[256];
    int song_id = 0;

    // DEFINING SQL EXECUTION CODE
    snprintf(query, 256, "SELECT rowid, title, artist FROM " MUSIC_TABLE
                         " WHERE title=\"%s\" COLLATE NOCASE AND artist=\"%s\" COLLATE NOCASE",
                         song.title, song.artist);

    // OPENING DB
    int rc = sqlite3_open(DB_FILENAME, &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open DB: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    // VALIDATING EXECUTION CODE
    rc = sqlite3_prepare_v2(db, query, -1, &res, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Error while finding song in DB: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -2;
    }

    // Checking if SQL execution returned anything
    rc = sqlite3_step(res);
    if (rc == SQLITE_ROW)
    {
        song_id = sqlite3_column_int(res,0);
    }

    sqlite3_finalize(res);
    sqlite3_close(db);
    return song_id;
}

int db_add_song(struct Song song, char *db_table)
{
    // Returns: int
    //          <0 -> error
    //          1  -> song already in library
    //          0  -> song added

    // Check if song is already in library
    int song_found = db_find_id_song(song);
    if (song_found < 0)
        // err
        return -1;
    else if (song_found > 0)
        // song already in library
        return 1;

    // Prepare execution code
    char query[256];
    snprintf(query, 256, "INSERT INTO %s (title, artist)"
                "VALUES (\"%s\", \"%s\")", db_table, song.title, song.artist);

    if (db_execute(query) == 0)
        return 0;
    else
    {
        printf("Unknown error occurred while adding song!\n");
        return -2;
    }
}

int db_query_song(int song_id) {
    char query[256];
    snprintf(query, 256, "INSERT INTO " QUERY_TABLE " (song_id) VALUES (%d)", song_id);

    if (db_execute(query) == 0)
        return 0;
    else
        return -1;
}

int db_download_missing_songs()
{
    sqlite3 *db;
    sqlite3_stmt *res;
    char query[256];
    int song_id = 0;

    // OPENING DB
    int rc = sqlite3_open(DB_FILENAME, &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open DB: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    // GET NUMBER OF MISSING SONGS
    snprintf(query, 256, "SELECT COUNT(*) FROM " MISSING_TABLE);
    rc = sqlite3_prepare_v2(db, query, -1, &res, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Error while counting missing songs: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -3;
    }
    rc = sqlite3_step(res);
    int num_missing = sqlite3_column_int(res, 0);
    clearBuffer(query, 256);
    sqlite3_finalize(res);

    // DEFINING SQL EXECUTION CODE
    Song missing_songs[num_missing];

    // VALIDATING EXECUTION CODE
    rc = sqlite3_prepare_v2(db, "SELECT title, artist FROM " MISSING_TABLE, -1, &res, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Error while getting songs from collection: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -2;
    }

    int i=0;
    while ((rc = sqlite3_step(res)) == SQLITE_ROW)
    {
        const char *title = (char*)sqlite3_column_text(res, 0);
        printf("-> DEBUG: TITLE: %s\n", title);
        strcpy(missing_songs[i].title, title);
        const char *artist = (char*)sqlite3_column_text(res, 1);
        printf("-> DEBUG: ARTIST: %s\n", artist);
        strcpy(missing_songs[i].artist, artist);
        i++;
    }

    sqlite3_finalize(res);
    sqlite3_close(db);

    // ADD ALL MISSING SONGS
    for (i=0; i<num_missing; i++)
        db_add_song(missing_songs[i], MUSIC_TABLE);

    // REMOVE ADDED SONGS FROM MISSING SONGS
    for (i=0; i<num_missing; i++)
    {
        snprintf(query, 256, "DELETE FROM " MISSING_TABLE " WHERE title=\"%s\" AND artist=\"%s\"",
                missing_songs[i].title, missing_songs[i].artist);
        db_execute(query);
    }

    return song_id;
}


int db_is_query_empty()
{
    sqlite3 *db;
    sqlite3_stmt *res;
    int status = -1;

    // OPENING DB
    int rc = sqlite3_open(DB_FILENAME, &db);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Cannot open DB: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -1;
    }

    // VALIDATING EXECUTION CODE
    rc = sqlite3_prepare_v2(db, "SELECT rowid FROM " QUERY_TABLE, -1, &res, 0);
    if (rc != SQLITE_OK)
    {
        fprintf(stderr, "Error while getting songs from collection: %s\n", sqlite3_errmsg(db));
        sqlite3_close(db);
        return -2;
    }
    rc = sqlite3_step(res);
    if (rc == SQLITE_ROW)
    {
        status = 0;
    }
    sqlite3_finalize(res);
    sqlite3_close(db);
    return status;
}