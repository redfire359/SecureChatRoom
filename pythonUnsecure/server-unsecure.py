'''
Our server will open a socket on port 443, waiting for incoming connections
it will then accept and print out any messages we receive 

'''
import socket 
import threading 

IP = '192.168.56.3'
port = 443

def connections(socket_connection):
    while True:
        try:
            message = socket_connection.recv(1024)
            if message:
                print("Got message")
                print(f"Message: {message.decode('utf-8')}")
        except Exception as e:
            print(f"Error: {e}")


try:
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((IP, port))
    server_socket.listen(3)
    print(f"Binded to port {port}")

    while True:
        socket_conn, address = server_socket.accept()
        print(f"Connection from: {address}")
        connections(socket_conn)

    server_socket.close()

except Exception as e:
    print(f"Exception occurred: {e}")
