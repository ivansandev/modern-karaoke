#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <string.h>

#include "db.h"

static int callback(void *NotUsed, int argc, char **argv, char **azColName)
{
    int i;
    for (i = 0; i < argc; i++)
    {
        printf("%s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
    }
    printf("\n");
    return 0;
}

int db_execute(char *query)
{
    // Funciton for executing given query for DB
    // Return: Status code (int)
    //         -1 -> cannot open DB
    //         -2 -> error while executing query
    //          0 -> OK
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

    // printf("DBG: Executed: %s\n", query);
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
            "length         REAL    NOT NULL DEFAULT 3.5,"
            "UNIQUE(title, artist))";
    // "YEAR           INTEGER"                      \
            // "BITRATE        INTEGER"                      \
            // "FILETYPE       TEXT"                         ;
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
            "title          TEXT    NOT NULL,"
            "artist         TEXT    NOT NULL,"
            "created_at     TEXT    NOT NULL DEFAULT CURRENT_TIMESTAMP,"
            "UNIQUE(title, artist))";
    int status = db_execute(query);
    if (status > 0)
        fprintf(stderr, "Error while creating 'query' table.\n");
    // else if (status == 0)
    //     printf("'query' table created!\n");
    return status;
}

int db_create_table_missing_songs()
{
    char *query;
    query = "CREATE TABLE IF NOT EXISTS " MISSING_TABLE
            "("
            "title          TEXT    NOT NULL,"
            "artist         TEXT    NOT NULL,"
            "UNIQUE(title, artist))";
    int status = db_execute(query);
    if (status > 0)
        fprintf(stderr, "Error while creating 'missing_songs' table.\n");
    // else if (status == 0)
    //     printf("'missing_songs' table created!\n");
    return status;
}

void initialize_db_tables()
{
    db_create_table_music();
    db_create_table_query();
    db_create_table_missing_songs();
}

int db_find_song(struct Song song, char *table)
{
    // Returns: int
    //          <0 -> error
    //           0 -> not found
    //           1 -> found
    sqlite3 *db;
    sqlite3_stmt *res;
    char query[256];
    int is_found = 0;

    // DEFINING SQL EXECUTION CODE
    snprintf(query, 256, "SELECT * FROM %s"
                         " WHERE title=\"%s\" COLLATE NOCASE AND artist=\"%s\" COLLATE NOCASE",
                         table, song.title, song.artist);

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
        is_found = 1;

    sqlite3_finalize(res);
    sqlite3_close(db);
    return is_found;
}

int db_add_song(struct Song song, char *db_table)
{
    // Returns: int
    //          <0 -> error
    //          1  -> song already in library
    //          0  -> song added

    // Check if song is already in library
    int song_found = db_find_song(song, db_table);
    if (song_found < 0)
        // err
        return -1;
    else if (song_found > 0)
        // song already in library
        return 1;

    char query[256];

    // Pointer comparison on string is ok here, since the db_table param is
    //     gonna be used from strictly defined params
    if (db_table == MUSIC_TABLE)
        snprintf(query, 256, "INSERT INTO %s (title, artist, length)"
                    "VALUES (\"%s\", \"%s\", %f)", db_table, song.title, song.artist, song.length);
    else
        snprintf(query, 256, "INSERT INTO %s (title, artist)"
                    "VALUES (\"%s\", \"%s\")", db_table, song.title, song.artist);

    int status = db_execute(query);
    if (status == 0)
        return 0;
    else
    {
        printf("Unknown error occurred while adding song!\n");
        return -2;
    }
}

// int request_song(struct Song song)
// {
//     // Returns: int
//     //          <0 -> error requesting song
//     //           0 -> Song found and requested
//     //           1 -> Song not found
//     int song_found = db_find_song(song, MUSIC_TABLE);
//     int status;
//     if (song_found < 0) {
//         printf("Error requesting song.\n");
//         return -1;
//     }
//     else if (song_found == 0)
//     {
//         // add_missing_song(song);
//         db_add_song(song, MISSING_TABLE);
//         return 1;
//     }
//     else {
//         db_add_song(song, QUERY_TABLE);
//         return 0;
//     }
// }

// void now_playing(char *current_song)
// {
//     sqlite3 *db;
//     sqlite3_stmt *res;
//     char query[256];

//     snprintf(query, 256, "SELECT * FROM " QUERY_TABLE " SORT ");
// }