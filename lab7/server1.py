import socket
import os

# Define the server address and port
SERVER_ADDRESS = ('127.0.0.1', 8080)

# Create a socket and bind it to the server address
server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
server_socket.bind(SERVER_ADDRESS)
server_socket.listen(1)
print("Bank server is waiting for connections...")

# Load user data from a file (username, encrypted password, balance)
user_data = {}

# Function to perform Caesar Cipher encryption
def encrypt(text):
    encrypted_text = ""
    for char in text:
        if char.isalpha():
            shift = 3
            encrypted_char = chr(((ord(char) - ord('a') + shift) % 26) + ord('a'))
            encrypted_text += encrypted_char
        else:
            encrypted_text += char
    return encrypted_text

# Load user data from a file (username, encrypted password, balance)
with open("bank_data.txt", "r") as file:
    for line in file:
        username, encrypted_password, balance_str = line.strip().split(',')
        user_data[username] = {
            'encrypted_password': encrypted_password,
            'balance': float(balance_str)
        }

while True:
    # Accept incoming connections
    client_socket, client_address = server_socket.accept()
    print(f"Connected to client {client_address}")

    # Receive and validate login credentials
    username = client_socket.recv(1024).decode()
    password = client_socket.recv(1024).decode()
    encrypted_password = encrypt(password)

    if username not in user_data:
        client_socket.send("Incorrect Username".encode())
    elif encrypted_password != user_data[username]['encrypted_password']:
        client_socket.send("Incorrect Password".encode())
    else:
        client_socket.send("Login Successful".encode())

        while True:
            # Send menu options to the client
            menu = "Menu:\n1. Debit\n2. Credit\n3. View Balance\n4. EXIT\nEnter your choice: "
            client_socket.send(menu.encode())
            choice = int(client_socket.recv(1024).decode())

            if choice == 1:  # Debit
                amount = float(client_socket.recv(1024).decode())
                if amount > user_data[username]['balance']:
                    client_socket.send("Insufficient balance".encode())
                else:
                    user_data[username]['balance'] -= amount
                    client_socket.send(f"Debited {amount} successfully".encode())
            elif choice == 2:  # Credit
                amount = float(client_socket.recv(1024).decode())
                user_data[username]['balance'] += amount
                client_socket.send(f"Credited {amount} successfully".encode())
            elif choice == 3:  # View Balance
                balance = user_data[username]['balance']
                client_socket.send(f"Balance: {balance}".encode())
            elif choice == 4:  # EXIT
                break

    # Close the connection
    print(f"Disconnected from client {client_address}")
    client_socket.close()
