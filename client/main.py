import socket

ADDR = "127.0.0.1"
PORT = "8080"

def main():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    status = s.connect(ADDR, PORT)
    print(status)


if __name__ == "__main__":
    main()