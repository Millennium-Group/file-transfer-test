import socket
from cryptography.fernet import Fernet

SERVER_HOST = '127.0.0.1'
SERVER_PORT = 12345
BUFFER_SIZE = 4096

def receive_file(conn):
    file_size = int.from_bytes(conn.recv(16), byteorder='big')
    encrypted_data = conn.recv(file_size)
    return fernet.decrypt(encrypted_data)

def save_file(data, filename):
    with open(filename, 'wb') as file:
        file.write(data)

def start_server():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.bind((SERVER_HOST, SERVER_PORT))
        server_socket.listen(1)
        print("Server listening...")
        conn, addr = server_socket.accept()
        
        # Generate a random key for encryption
        key = Fernet.generate_key()
        global fernet
        fernet = Fernet(key)
        
        print(f"Connection from {addr}")
        
        received_data = receive_file(conn)
        save_file(received_data, "received_file.txt")
        print("File received and saved as 'received_file.txt'")
        conn.close()

if __name__ == "__main__":
    start_server()