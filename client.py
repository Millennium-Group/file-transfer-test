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

def send_encrypted_key(connection, key, server_public_key):
    try:
        encrypted_key = server_public_key.encrypt(
            key,
            padding.OAEP(
                mgf=padding.MGF1(algorithm=hashes.SHA256()),
                algorithm=hashes.SHA256(),
                label=None
            )
        )
        connection.sendall(encrypted_key)
    except Exception as e:
        print(f"Error sending encrypted key: {e}")
        raise

def encrypt_and_send_file(connection, key, file_path):
    try:
        cipher = Cipher(algorithms.AES(key), modes.CFB(os.urandom(16)), backend=default_backend())

        with open(file_path, 'rb') as file:
            while True:
                data = file.read(4096)
                if not data:
                    break
                encryptor = cipher.encryptor()
                encrypted_data = encryptor.update(data) + encryptor.finalize()
                connection.sendall(encrypted_data)

    except Exception as e:
        print(f"Error encrypting or sending file: {e}")
        raise

def secure_client(file_path):
    host = 'localhost'
    port = 8888

    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
            client_socket.connect((host, port))

            private_key, public_key = generate_keypair()
            server_public_key_bytes = client_socket.recv(4096)
            server_public_key = serialization.load_pem_public_key(
                server_public_key_bytes,
                backend=default_backend()
            )

            key = os.urandom(32)
            send_encrypted_key(client_socket, key, server_public_key)
            encrypt_and_send_file(client_socket, key, file_path)

            print(f"File '{file_path}' sent to the server securely.")

    except Exception as e:
        print(f"An unexpected error occurred: {e}")

if __name__ == "__main__":
    secure_client('/home/.../Downloads/README.txt')  # Replace with the actual file path
