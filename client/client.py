#!/usr/bin/env python3
import socket
import time
import sys

ADDR = "127.0.0.1"
PORT = 8080
PACKAGE_LEN = 1024

# Server Response Status
ERROR_MSG            = "ERROR"
SONG_UNAVAILABLE_MSG = "SNGNA"
SONG_REQUESTED_MSG   = "SNGOK"


def recv_ok(s):
    msg = s.recv(2, 0).decode()
    if msg != "OK":
        return False
    return True


def handshake() -> bool:
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            # Connect to socket
            status = s.connect((ADDR, PORT))

            # Send handshake
            s.send(str.encode("HANDSHAKE\0"), 0)

            # Receive handshake
            if not recv_ok(s):
                return False
            else:
                return True

    except ConnectionRefusedError:
        # No connection to server
        return False


def request_song(title, artist) -> int:
    if not title or title == "" or not artist or artist == "":
        return -2
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:

            # Connect to socket
            status = s.connect((ADDR, PORT))

            # Send type of request
            s.send(str.encode("REQUEST\0"), 0)
            if not recv_ok(s):
                print("OK not received")
                return -5

            # Send title
            s.send(str.encode(title))
            if not recv_ok(s):
                print("OK not received")
                return -5

            # Send artist
            s.send(str.encode(artist))
            if not recv_ok(s):
                print("OK not received")
                return -5

            # Get response
            msg = s.recv(5, 0).decode()
            if (msg == SONG_REQUESTED_MSG):
                return 0

            elif (msg == SONG_UNAVAILABLE_MSG):
                return 1

            elif (msg == ERROR_MSG):
                return -3

            else:
                return -4

    except ConnectionRefusedError:
        # No connection to server
        return -1

def request_next_playing():
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:

            # Connect to socket
            status = s.connect((ADDR, PORT))

            # Send type of request
            s.send(str.encode("NEXT\0"), 0)

            # Get response
            msg = s.recv(5, 0).decode()
            return msg;

    except ConnectionRefusedError:
        # No connection to server
        return -1


if __name__ == "__main__":
    # For running in CLI without GUI
    if len(sys.argv) > 2:
        title = sys.argv[1]
        artist = sys.argv[2]
        res = request_song(title, artist)
        if (res == 1):
            print("Song not available.")
        elif (res == 0):
            print("Song requested.")
        elif (res < -1):
            print("Error")
    else:
        print("Insufficent args.")
