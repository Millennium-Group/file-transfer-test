import socket
import os
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
import ctypes

SERVER_HOST = '127.0.0.1'
SERVER_PORT = 12345
BLOCK_SIZE = 1024  # Define the block size for file transfer

def decrypt_file(file_path, key, iv, ciphertext):
    """
    Decrypt the received file using the provided key and IV.
    """
    try:
        cipher = Cipher(algorithms.AES(key), modes.CFB(iv), backend=default_backend())
        decryptor = cipher.decryptor()
        plaintext = decryptor.update(ciphertext) + decryptor.finalize()

        with open(file_path, 'wb') as file:
            file.write(plaintext)

        # Securely wipe the plaintext and key from memory
        ctypes.memset(plaintext, 0, len(plaintext))
        ctypes.memset(key, 0, len(key))
    except Exception as e:
        print(f"Error during file decryption: {e}")

def receive_file(file_path, key):
    """
    Receive the encrypted file from the client and decrypt it.
    """
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.bind((SERVER_HOST, SERVER_PORT))
        s.listen(1)

        conn, addr = s.accept()

        iv = conn.recv(16)
        ciphertext = b''
        data = conn.recv(BLOCK_SIZE)
        while data:
            ciphertext += data
            data = conn.recv(BLOCK_SIZE)

        conn.close()
        s.close()

        decrypt_file(file_path, key, iv, ciphertext)
    except Exception as e:
        print(f"Error during file reception: {e}")

if __name__ == "__main__":
    file_path = 'received_file'
    key = b''  # Use the same key used for encryption on the client side
    receive_file(file_path, key)
