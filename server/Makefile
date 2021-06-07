main:
	mkdir -p db
	mkdir -p build
	gcc -Wall -pthread main.c db/db.c src/player.c src/menu_functions.c src/client_requests.c src/helpers.c ext/sqlite/sqlite3.c -lpthread -o build/out
