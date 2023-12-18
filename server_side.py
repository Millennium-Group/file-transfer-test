import socket
from cryptography.fernet import Fernet
import signal
import sys

server_socket = None


def receive_all(sock, count):
    data = b""
    while len(data) < count:
        packet = sock.recv(count - len(data))
        if not packet:
            return None
        data += packet
    return data


def signal_handler(sig, frame):
    global server_socket
    print("\nStopping the server.")
    if server_socket:
        server_socket.close()
    sys.exit(0)


def receive_encrypted_file(client_socket):
    # Receive the key from the client
    received_key = client_socket.recv(1024)

    print("Received key from client")

    # Receive file size
    file_size_data = receive_all(client_socket, 1024)
    if file_size_data is None:
        raise ValueError("Failed to receive file size data")

    file_size = int(file_size_data.decode())
    print("Received file size:", file_size)

    # Acknowledge the file size receipt
    client_socket.send(b"Size received")
    print("Acknowledgment sent")

    # Receive and write the encrypted file
    received_data = b""
    while len(received_data) < file_size:
        packet = client_socket.recv(1024)
        if not packet:
            break
        received_data += packet

    print("Received encrypted data")

    # Decrypt the received data using the key received from the client
    cipher_suite = Fernet(received_key)
    decrypted_data = cipher_suite.decrypt(received_data)

    # Store decrypted data in a file
    with open("received_file.txt", "wb") as file:
        file.write(decrypted_data)

    print("File received successfully")


def main():
    global server_socket
    signal.signal(signal.SIGINT, signal_handler)
    server_socket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    host = "127.0.0.1"
    port = 12345

    server_socket.bind((host, port))
    server_socket.listen(5)

    print(f"Server listening on {host}:{port}")
    try:
        while True:
            client_socket, addr = server_socket.accept()
            print("Connection from", addr)

            try:
                receive_encrypted_file(client_socket)
            except Exception as e:
                print("Error occurred during file transfer:", e)

            client_socket.close()

    except KeyboardInterrupt:  # Catch KeyboardInterrupt (Ctrl+C)
        print("\nStopping the server.")
        if server_socket:
            server_socket.close()
        sys.exit(0)


if __name__ == "__main__":
    main()
