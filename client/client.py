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

def main():
    request_song("animals", "martin garrix")

def recv_ok(s):
    msg = s.recv(2, 0).decode()
    if msg != "OK":
        return False
    return True

def request_song(title, artist) -> int:
    if not title or title == "" or not artist or artist == "":
        return -2
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            # s.setblocking(0)

            # Connect to socket
            status = s.connect((ADDR, PORT))

            # Send type of request
            print("DEBUG: Sending request type")
            s.send(str.encode("REQUEST\0"), 0)
            # time.sleep(0.01)
            if not recv_ok(s):
                print("OK not received")

            # Send title
            print("DEBUG: Sending song title")
            s.send(str.encode(title))
            # time.sleep(0.01)
            if not recv_ok(s):
                print("OK not received")

            # Send artist
            print("DEBUG: Sending song artist")
            s.send(str.encode(artist))
            # time.sleep(0.01)
            if not recv_ok(s):
                print("OK not received")

            # Get response
            msg = s.recv(5, 0).decode()
            if (msg == SONG_REQUESTED_MSG):
                # print("Request successful.")
                return 0
            elif (msg == SONG_UNAVAILABLE_MSG):
                # print("Song unavailable. Please try another song.")
                return 1
            elif (msg == ERROR_MSG):
                # print("Error occured while requesting song.")
                return -3
            else:
                # print("Unknown response.")
                return -4


    except ConnectionRefusedError:
        print("Cannot connect to server.")
        return -1

if __name__ == "__main__":
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
