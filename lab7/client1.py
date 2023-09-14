import socket

# Define the server address and port
SERVER_ADDRESS = ('127.0.0.1', 8080)

# Create a socket and connect to the server
client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
client_socket.connect(SERVER_ADDRESS)

# Get user login credentials
username = input("Enter username: ")
password = input("Enter password: ")

# Send login credentials to the server
client_socket.send(username.encode())
client_socket.send(password.encode())

# Receive and print the server's response
response = client_socket.recv(1024).decode()
print(response)

if response == "Login Successful":
    while True:
        choice = int(input("Enter your choice (1-Debit, 2-Credit, 3-View Balance, 4-EXIT): "))
        client_socket.send(str(choice).encode())

        if choice == 1 or choice == 2:
            amount = float(input("Enter the amount: "))
            client_socket.send(str(amount).encode())

        server_response = client_socket.recv(1024).decode()
        print(server_response)

        if choice == 4:
            break

# Close the client socket
client_socket.close()
