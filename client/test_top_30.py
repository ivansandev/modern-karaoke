#!/usr/bin/env python3

import billboard
import client

def main():
    client_t = list()
    chart = billboard.ChartData('hot-100')[0:30]
    for song in chart:
        client.request_song(song.title, song.artist)


if __name__ == "__main__":
    main()