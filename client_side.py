import socket
from cryptography.fernet import Fernet


def generate_key():
    return Fernet.generate_key()


def encrypt_file(file_content, key):
    cipher_suite = Fernet(key)
    return cipher_suite.encrypt(file_content)


def send_file_to_server(server_host, server_port):
    client_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    client_socket.connect((server_host, server_port))

    try:
        file_path = "file_to_transfer.txt"
        with open(file_path, "rb") as file:
            file_content = file.read()
            file_size = len(file_content)

        # Generate key for encryption
        key = generate_key()
        print("Generated key")

        # Send the key to the server
        client_socket.send(key)
        print("Sent key to server")

        # Send file size to the server
        client_socket.send(
            str(file_size).encode().ljust(1024)
        )  # Ensure fixed-size data
        print("Sent file size to server")

        # Wait for acknowledgment from the server
        ack = client_socket.recv(1024)
        if ack != b"Size received":
            raise ValueError("Failed to receive acknowledgment for file size")
        print("Received acknowledgment from server")

        # Encrypt file content
        encrypted_data = encrypt_file(file_content, key)

        # Send encrypted file
        client_socket.sendall(encrypted_data)
        print("Sent encrypted data to server")

        print("File sent successfully!")

    except Exception as e:
        print("Error occurred:", e)

    finally:
        client_socket.close()


def main():
    server_host = "127.0.0.1"
    server_port = 12345

    send_file_to_server(server_host, server_port)


if __name__ == "__main__":
    main()
