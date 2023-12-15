# file-transfer-test

This Python module demonstrates a basic implementation of a secure file transfer system using Python sockets and AES encryption.

## Overview

The module consists of two main components: `server.py` and `client.py`. These components enable secure file transfer between a server and a client using a basic encryption mechanism (AES encryption from the `cryptography` library) over a network.

### Files

- `server.py`: Contains the server-side code that listens for incoming connections from clients and receives encrypted files sent by clients. It decrypts the received file and saves it as `received_file.txt`.

- `client.py`: Contains the client-side code that establishes a connection with the server, encrypts a file (`file_to_send.txt`), and sends it to the server for storage.

## Usage

1. Clone the repository:

    ```bash
    git clone https://github.com/nadirhamid/file-transfer-test.git
    ```

2. Install the required library:

    ```bash
    pip install cryptography
    ```

3. Run the server:

    ```bash
    python server.py
    ```

4. Run the client:

    ```bash
    python client.py
    ```

5. Verify the process:

    - After running the client, the server should display a message indicating the successful receipt of the file (`received_file.txt`).

## Design Decisions

- **Encryption Mechanism**: AES encryption was chosen due to its security and speed for this demonstration. The `cryptography` library provides a simple interface for implementing AES encryption.

- **Socket Communication**: Python sockets were utilized for network communication between the client and server. This offers a straightforward way to establish connections and transfer data.

- **File Handling**: The module supports basic file transfer, handling both small and large files efficiently.

## Challenges Faced

- **Memory Management**: Ensuring secure memory allocation and deallocation practices while handling encryption keys and sensitive data was challenging. The `cryptography` library helped manage encryption keys securely.

- **Error Handling**: Robust error handling for potential network issues and security concerns required thorough consideration. The current implementation lacks comprehensive error management for various scenarios.

## Note

- This is a basic implementation for a test assessment and doesn't cover all security aspects or error handling for a production environment.

- Consider enhancing the codebase with more robust error handling, security measures, and additional features for a real-world application.

- Please ensure that you have appropriate permissions for sending and receiving files in the specified directories.