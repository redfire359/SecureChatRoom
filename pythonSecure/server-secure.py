import socket
import ssl
import threading

# Server configuration
SERVER_HOST = '192.168.56.3'
SERVER_PORT = 443
SERVER_CERT = './server.crt'
SERVER_KEY = './ca.key'
CA_CERT = './ca.pem'  # Use the same CA certificate

def handle_client(client_socket, client_address):
    print(f"[+] New connection from {client_address}")
    
    try:
        with client_socket:
            while True:
                message = client_socket.recv(1024).decode('utf-8')
                if not message:
                    break
                print(f"[{client_address}] {message}")
                client_socket.sendall(b"Message received")
    except Exception as e:
        print(f"[!] Exception: {e}")

def start_server():
    context = ssl.create_default_context(ssl.Purpose.CLIENT_AUTH)
    context.load_cert_chain(certfile=SERVER_CERT, keyfile=SERVER_KEY)
    context.load_verify_locations(cafile=CA_CERT)
    context.verify_mode = ssl.CERT_REQUIRED  # Require client certificate verification

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind((SERVER_HOST, SERVER_PORT))
        server_socket.listen(5)
        print(f"[*] Listening on {SERVER_HOST}:{SERVER_PORT}")

        while True:
            client_socket, client_address = server_socket.accept()
            client_socket = context.wrap_socket(client_socket, server_side=True)
            client_handler = threading.Thread(target=handle_client, args=(client_socket, client_address))
            client_handler.start()

if __name__ == "__main__":
    start_server()

