#ifndef CLIENT_MODULE
#define CLIENT_MODULE

#include <stdio.h>
#include <stdlib.h>
#include "db.h"

int request_song(struct Song song);

int request_now_playing();

void now_playing(char *buffer);

#endif /* CLIENT_MODULE */