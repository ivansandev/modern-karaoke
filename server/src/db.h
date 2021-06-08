#ifndef DB_MODULE
#define DB_MODULE

#define DB_FILENAME "db/music.db"
#define MUSIC_TABLE "collection"
#define MISSING_TABLE "missing_requests"
#define QUERY_TABLE "requests"

typedef struct Song {
    char title[100];
    char artist[100];
    char genre[50];
} Song;

// static int callback(void *NotUsed, int argc, char **argv, char **azColName);
int db_execute(char *query);

// DB CONFIGURATION
void initialize_db();
int db_create_table_music();
int db_create_table_missing_songs();
int db_create_table_query();

int db_find_id_song(struct Song song);
int db_add_song(struct Song song, char *db_table);
int db_remove_song(char *title_or_artist);
int db_query_song(int song_id);
int db_download_missing_songs();
int db_is_query_empty();

#endif /* DB_MODULE */