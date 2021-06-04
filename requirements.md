27.
Асистентът по СПр много обича да ходи на караоке, обаче не какво да е, а с модерни техники и уредби.
Съответно той си поръчва през дадено приложение името на изпълнител и името на песен, а DJ Тошко си пази при себе си целия каталог от парчета.
Ако парчето го има, асистентът го пее, ако го няма - поръчва си друго. Когато му писне, си тръгва от заведението, но Тошето е пич и при новото посещение, вече е намерил парчето, а асистентът е щастлив и си го изпява с кеф!


# Explanation
----
Main idea --> Karaoke app, for requesting a song

Client(s) requests a song (by artist and title), the server (DJ) sees the whole catalogue of songs (e.g. in DB, SQLite).

	* If the requested song is available   --> gets added to a QUEUE_LIST table in DB.   --> notifies client

	* If the requested song is unavailable --> gets added to a DOWNLOAD_LIST table in DB --> notifies client

Server has thread which is playing a song at every X minutes. It goes over QUEUE_LIST table, but if QUEUE_LIST table is empty, it picks random song from library.

Signal handler -> when client lives (program interrupted).

Server can stop party (from the menu, or from SIGINT) --> client(s) needs to get notified about it


Requirements:
- server-client app
- server listening to port 8080
- new thread on server with TCP socket connection for every client
- DB: SQLite
- Files: now_playing
	- critical-section --> DB, now_playing file


# Research:
## Server:
	[ ] Sockets
	[ ] Threads
	[ ] Processes
		[ ] Communication between them processes
## Client
	[ ] Sockets in Python
	[ ] GUI in Python
	[ ]

Possibilities:
- Server is started
	- Party started
		- Client connects, server creates new socket in a new thread and listens to socket messages
		- Client can send to server
			- Request song
				- Title
				- Artist
			- Now playing
			- Leave party (signal handler)

# To-Do:
## Client
[ ] GUI in Python
## Server
[ ] Proper TDP socket (server & client)
[ ] Implement encoding/decoding of message requests
[ ] Update server with mutex
[ ] Implement signals
[ ] Player thread (started on start_party, stopped on end_party/signal)
[ ] Menu functionalities:
	[ ] Show all available songs
	[ ] Show requests
	[ ] Show missing songs


# Testing:
[ ] Multiple users requesting at once
