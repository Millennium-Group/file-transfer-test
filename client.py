import socket
from cryptography.fernet import Fernet

SERVER_HOST = '127.0.0.1'
SERVER_PORT = 12345
BUFFER_SIZE = 4096

def encrypt_file(file_path, key):
    with open(file_path, 'rb') as file:
        data = file.read()
    fernet = Fernet(key)
    return fernet.encrypt(data)

def send_file(conn, file_path):
    encrypted_data = encrypt_file(file_path, key)
    conn.sendall(len(encrypted_data).to_bytes(16, byteorder='big'))
    conn.sendall(encrypted_data)

def start_client():
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
        client_socket.connect((SERVER_HOST, SERVER_PORT))
        
        # Use the same key for encryption/decryption
        global key
        key = Fernet.generate_key()
        
        send_file(client_socket, 'file_to_send.txt')
        print("File sent successfully")
        client_socket.close()

if __name__ == "__main__":
    start_client()