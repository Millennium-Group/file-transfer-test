import socket
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives import serialization, hashes
from cryptography.hazmat.primitives.asymmetric import rsa, padding
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
import os

def generate_keypair():
    try:
        private_key = rsa.generate_private_key(
            public_exponent=65537,
            key_size=2048,
            backend=default_backend()
        )
        public_key = private_key.public_key()
        return private_key, public_key
    except Exception as e:
        print(f"Error generating key pair: {e}")
        raise

def send_public_key(connection, public_key):
    try:
        serialized_key = public_key.public_bytes(
            encoding=serialization.Encoding.PEM,
            format=serialization.PublicFormat.SubjectPublicKeyInfo
        )
        connection.sendall(serialized_key)
    except Exception as e:
        print(f"Error sending public key: {e}")
        raise

def receive_encrypted_key(connection, private_key):
    try:
        encrypted_key = connection.recv(4096)
        if not encrypted_key:
            raise ValueError("No data received for the encrypted key")

        decrypted_key = private_key.decrypt(
            encrypted_key,
            padding.OAEP(
                mgf=padding.MGF1(algorithm=hashes.SHA256()),
                algorithm=hashes.SHA256(),
                label=None
            )
        )
        return decrypted_key
    except ValueError as ve:
        print(f"Error: {ve}")
        raise
    except Exception as e:
        print(f"Error receiving or decrypting key: {e}")
        raise

def receive_and_decrypt_file(connection, key, file_path):
    try:
        cipher = Cipher(algorithms.AES(key), modes.CFB(os.urandom(16)), backend=default_backend())

        with open(file_path, 'wb') as file:
            while True:
                encrypted_data = connection.recv(4096)
                if not encrypted_data:
                    break
                decryptor = cipher.decryptor()
                decrypted_data = decryptor.update(encrypted_data) + decryptor.finalize()
                file.write(decrypted_data)

    except Exception as e:
        print(f"Error receiving or decrypting file: {e}")
        raise

def secure_server():
    host = 'localhost'
    port = 8888

    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
            server_socket.bind((host, port))
            server_socket.listen()

            print(f"Server listening on {host}:{port}")

            client_socket, addr = server_socket.accept()
            with client_socket:
                print(f"Connection established from {addr}")

                private_key, public_key = generate_keypair()
                send_public_key(client_socket, public_key)

                key = receive_encrypted_key(client_socket, private_key)
                file_path = '/home/.../Downloads/received_file.txt'
                receive_and_decrypt_file(client_socket, key, file_path)

                print(f"File received and decrypted: {file_path}")

    except Exception as e:
        print(f"An unexpected error occurred: {e}")

if __name__ == "__main__":
    secure_server()
