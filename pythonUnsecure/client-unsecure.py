#!/usr/bin/python3
'''
Python Server Code 

'''
# stop the __pychache__ folder 
import sys
sys.dont_write_bytecode = True

import socketserver
import socket
import os
import threading 

IP = '192.168.56.3'
port = 443
num_connections = 4 

client_socket = socket.socket()
client_socket.connect((IP, port))

print('[*] Listening...')

def recv(client_socket):
    message = client_socket.recv(1024)

    while True:
        if message:
            print(msg.decode('utf-8'))
        else:
            client_socket.close()
            break
        

def main():
    
    client_socket, address = server_socket.accept()
    print(f"Connections from: {address}")
    
    t1 = threading.Thread(target=recv, name='t1', args=[client_socket]).start()
    
    while True:

        userInput = input("Send: ")
        message = userInput.encode('utf-8')
        client_socket.send(message)

    server_socket.close()

if __name__ == "__main__":
    main()
    