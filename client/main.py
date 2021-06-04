import socket
import time
import sys

ADDR = "127.0.0.1"
PORT = 8080

# Messages
MAX_CLIENT_MSG = 1000
ERROR_MSG            = "ERROR"
SONG_UNAVAILABLE_MSG = "SNGNA"
SONG_REQUESTED_MSG   = "SNGOK"

def main():
    request_song("animals", "martin garrix")

def request_song(title, artist):
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            # Connect to socket
            status = s.connect((ADDR, PORT))

            # Send type of request
            s.send(str.encode("REQUEST\0"), 0)
            time.sleep(1)

            # Send title
            s.send(str.encode(title))
            time.sleep(1)

            # Send artist
            s.send(str.encode(artist))
            time.sleep(1)

            # Get response
            msg = s.recv(5, 0).decode()
            if (msg == SONG_REQUESTED_MSG):
                print("Request successful.")
            elif (msg == SONG_UNAVAILABLE_MSG):
                print("Song unavailable. Please try another song.")
            elif (msg == ERROR_MSG):
                print("Error occured while requesting song.")
            else:
                print("Unknown response.")


    except ConnectionRefusedError:
        print("Cannot connect to server.")
    except KeyboardInterrupt:
        print("Exiting...")

if __name__ == "__main__":
    if sys.argv.count() > 2:
        title = sys.argv[1]
        artist = sys.argv[2]
        request_song(title, artist)

    # main()
