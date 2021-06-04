#ifndef DB_MODULE
#define DB_MODULE

#define DB_FILENAME "db/music.db"
#define MUSIC_TABLE "song_collection"
#define MISSING_TABLE "missing_songs"
#define QUERY_TABLE "query_playlist"

typedef struct Song {
    char title[100];
    char artist[100];
    float length;
} Song;

static int callback(void *NotUsed, int argc, char **argv, char **azColName);
int db_execute(char *query);

void initialize_db_tables();
int db_create_table_music();
int db_create_table_missing_songs();
int db_create_table_query();

// TODO: Methods below


// CLIENT REQUEST
// int request_song(struct Song song);      // done
// void now_playing(char *buffer);

// FUNCTIONS FOR CLIENT REQUEST
int db_find_song(struct Song song, char *table); // done

// SERVER FUNCTIONS
int db_add_song(struct Song song, char *db_table);         // done
int db_remove_song(char *title_or_artist);
int db_add_missing_songs();

void download_missing_songs();

#endif /* DB_MODULE */