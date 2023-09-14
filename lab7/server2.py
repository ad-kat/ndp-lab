import socket
import threading

# Define the server address and port
SERVER_IP = '0.0.0.0'  # Use the server's IP address
SERVER_PORT = 12345

# Create a socket for the server
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind((SERVER_IP, SERVER_PORT))
server_socket.listen(1)

print(f"Server is listening on {SERVER_IP}:{SERVER_PORT}")

# List to store connected clients
clients = []

# Function to broadcast messages to all clients
def broadcast(message, client_socket):
    for client in clients:
        if client != client_socket:
            try:
                client.send(message)
            except:
                # Remove the client if the send operation fails
                clients.remove(client)

# Function to handle each client
def client_handler(client_socket):
    while True:
        try:
            message = client_socket.recv(1024)
            if not message:
                break
            broadcast(message, client_socket)
        except:
            continue

    # Close the client connection
    client_socket.close()

# Accept and handle incoming client connections
while True:
    client_conn, client_addr = server_socket.accept()
    print(f"Connected to {client_addr}")
    clients.append(client_conn)

    # Start a thread to handle the client
    client_thread = threading.Thread(target=client_handler, args=(client_conn,))
    client_thread.start()
