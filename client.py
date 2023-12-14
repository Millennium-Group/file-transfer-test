import socket
import os
import cryptography
from cryptography.hazmat.primitives import hashes
from cryptography.hazmat.primitives.kdf.pbkdf2 import PBKDF2HMAC
from cryptography.hazmat.backends import default_backend
from cryptography.hazmat.primitives.ciphers import Cipher, algorithms, modes
import ctypes

SERVER_HOST = '127.0.0.1'
SERVER_PORT = 12345
BLOCK_SIZE = 1024  # Define the block size for file transfer

def generate_key(password, salt):
    kdf = PBKDF2HMAC(
        algorithm=hashes.SHA256(),
        length=32,
        salt=salt,
        iterations=100000,
        backend=default_backend()
    )
    key = kdf.derive(password)
    # Securely wipe the password from memory
    ctypes.memset(password, 0, len(password))
    return key

def encrypt_file(file_path, key):
    """
    Encrypt the file using AES algorithm in CFB mode.
    """
    try:
        with open(file_path, 'rb') as file:
            plaintext = file.read()

        iv = os.urandom(16)
        cipher = Cipher(algorithms.AES(key), modes.CFB(iv), backend=default_backend())
        encryptor = cipher.encryptor()
        ciphertext = encryptor.update(plaintext) + encryptor.finalize()

        # Securely wipe the plaintext and key from memory
        ctypes.memset(plaintext, 0, len(plaintext))
        ctypes.memset(key, 0, len(key))

        return iv, ciphertext
    except Exception as e:
        print(f"Error during file encryption: {e}")
        return None, None

def send_file(file_path, key):
    """
    Send the encrypted file to the server.
    """
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((SERVER_HOST, SERVER_PORT))

        iv, ciphertext = encrypt_file(file_path, key)
        if iv is not None and ciphertext is not None:
            # Send the IV and ciphertext
            s.sendall(iv)
            s.sendall(ciphertext)

        s.close()
    except Exception as e:
        print(f"Error during file transmission: {e}")

def resume_file_transfer(file_path, key):
    """
    Resume interrupted file transfer by sending the remaining blocks.
    """
    try:
        s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        s.connect((SERVER_HOST, SERVER_PORT))

        with open(file_path, 'rb') as file:
            # Seek to the last transferred position
            file.seek(os.path.getsize(file_path))

            while True:
                data = file.read(BLOCK_SIZE)
                if not data:
                    break
                s.sendall(data)

        s.close()
    except Exception as e:
        print(f"Error during file transmission: {e}")

if __name__ == "__main__":
    file_path = 'file_to_send'
    password = b'your_password_here'
    salt = os.urandom(16)
    key = generate_key(password, salt)
    send_file(file_path, key)
    # If the transfer was interrupted, resume from the last position
    resume_file_transfer(file_path, key)
