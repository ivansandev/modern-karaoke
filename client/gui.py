import tkinter as tk
import main as app

window = tk.Tk()

def handle_request_but(event):
    title=ent_title.get()
    artist=ent_artist.get()
    res = app.request_song(title, artist)
    notification_window = tk.Tk()
    if (res == 1):
        label = tk.Label(master=notification_window, text="Song not available.", height=3, width=30, bg="organe", fg="black")
        label.pack()
    elif (res == 0):
        label = tk.Label(master=notification_window, text="Song requested.", height=3, width=30, bg="lightgreen", fg="black")
        label.pack()
    elif (res < -1):
        label = tk.Label(master=notification_window, text="Error!", height=2, width=10, bg="red", fg="black")
        label.pack()

# WELCOME LABEL
lbl_welcome = tk.Label(
    text="Welcome to ModernCLUB",
    fg="white",
    bg="black",
    width=30,
    height=3
    )
lbl_welcome.pack()

# NOW PLAYING BUTTON
but_now_playing = tk.Button(
    text="Now Playing?",
    width=12,
    height=2,
    bg="blue",
    fg="black"
)
but_now_playing.pack()

# TITLE (for song request)
lbl_title = tk.Label(text="Title")
ent_title = tk.Entry(
    width=30,
)
lbl_title.pack()
ent_title.pack()

# ARTIST (for song request)
lbl_artist = tk.Label(text="Artist")
ent_artist = tk.Entry(
    width=30,
)
lbl_artist.pack()
ent_artist.pack()

# SONG REQUEST BUTTON
but_request = tk.Button(
    text="Request Song",
    width=12,
    height=2,
    bg="blue",
    fg="black"
)
but_request.bind("<Button-1>", handle_request_but)
but_request.pack()

window.mainloop()