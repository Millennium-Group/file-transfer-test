import socket
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.backends import default_backend

#Creates a client socket and connects to the specified host and port.
def start_client(host, port):
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((host, port))
    return client_socket

#Closes the given socket.
def close_socket(socket):
    socket.close()

#Reads a file and encrypts its content using the specified key and 
#an initialization vector. Returns the encrypted content.
def encrypt_file(file_path, key):
    with open(file_path, 'rb') as file:
        plaintext = file.read()
    iv = b'0123456789012345'  # Initialization vector
    cipher = Cipher(algorithms.AES(key), modes.CFB8(iv), backend=default_backend())
    encryptor = cipher.encryptor()
    ciphertext = encryptor.update(plaintext) + encryptor.finalize()
    return ciphertext

#Sends the encrypted content of a file to the server using the client socket.
def send_file(client_socket, file_path, key):
    try:
        ciphertext = encrypt_file(file_path, key)
        client_socket.send(ciphertext)
        print(f"File '{file_path}' sent successfully.")
    except Exception as e:
        print(f"Error sending file: {e}")
    finally:
        close_socket(client_socket)

#The main function that orchestrates the entire process. It starts the client, 
#sends the file, and handles any exceptions that may occur during the process.
def main():
    host = '127.0.0.1'
    port = 12345

    client_socket = None

    try:
        client_socket = start_client(host, port)
        send_file(client_socket, 'file_to_send.txt', b'key1234567890123')

    except Exception as e:
        print(f"Error: {e}")

if __name__ == "__main__":
    main()

