main:
	mkdir -p db
	mkdir -p build
	gcc -Wall -pthread main.c db/db.c player.c menu_functions.c client_requests.c helpers.c ext/sqlite/sqlite3.c -lpthread -o build/out
