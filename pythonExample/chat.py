#!/usr/bin/python3

# stop the __pychache__ folder 
import sys
sys.dont_write_bytecode = True

import socketserver
import socket
import os

server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind(('192.168.56.3', 443))
server_socket.listen(5)


print('Listening...')


# variables
x = ''

while True:
    try:
        client_socket, address = server_socket.accept()
        print(f"Connection from {address}")
    except KeyboardInterrupt:
        client_socket.close()
        break

    while True:
        try:
            msg = input("send: ")
            msg1 = msg.encode('utf-8')
            client_socket.send(msg1)
        except KeyboardInterrupt:
            client_socket.close()
            exit(0)

client_socket.close()
print("\nClosed socket...")



