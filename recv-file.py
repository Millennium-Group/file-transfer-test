import socket
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
from cryptography.hazmat.backends import default_backend

#Creates a server socket, binds it to the specified host and port, 
#and listens for a connection. Returns both the server socket
#and the connected client socket.
def start_server(host, port):
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    server_socket.bind((host, port))
    server_socket.listen(1)
    print(f"Server listening on {host}:{port}")
    client_socket, client_address = server_socket.accept()
    return server_socket, client_socket

#Closes the given socket.
def close_socket(socket):
    socket.close()

#Decrypts the provided ciphertext using the specified key 
#and an initialization vector. Returns the decrypted content.
def decrypt_file(ciphertext, key):
    iv = b'0123456789012345'  # Initialization vector
    cipher = Cipher(algorithms.AES(key), modes.CFB8(iv), backend=default_backend())
    decryptor = cipher.decryptor()
    plaintext = decryptor.update(ciphertext) + decryptor.finalize()
    return plaintext

#Receives the encrypted content of a file from the client using the server socket, 
#decrypts it, and saves it to a file.
def receive_file(client_socket, file_path, key):
    try:
        ciphertext = client_socket.recv(4096)
        plaintext = decrypt_file(ciphertext, key)
        with open(file_path, 'wb') as file:
            file.write(plaintext)
        print(f"File received and saved as '{file_path}'.")
    except Exception as e:
        print(f"Error receiving file: {e}")
    finally:
        close_socket(client_socket)

#The main function that orchestrates the entire process. 
#It starts the server, receives the file, and handles any 
#exceptions that may occur during the process.
def main():
    host = '127.0.0.1'
    port = 12345

    client_socket = None
    server_socket = None

    try:
        server_socket, client_socket = start_server(host, port)
        receive_file(client_socket, 'received_file.txt', b'key1234567890123')

    except Exception as e:
        print(f"Error: {e}")
    finally:
        if server_socket:
            close_socket(server_socket)

if __name__ == "__main__":
    main()

