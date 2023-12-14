Requirements
- Python 3.x
- cryptography library (install using pip install cryptography)

Configuration
- Update the "file_to_send.txt" with the name of the file you want to send.
- Update the b'key1234567890123' with a strong and secure encryption key.



```markdown
# recv-file.py

This script receives an encrypted file on a server from a client using a Python socket. The received file is decrypted using the AES encryption algorithm with a specified key.

## Usage

1. Ensure that Python is installed on both the client and server machines.

2. Run the server script on the machine that will receive the file:

   ```bash
   python recv-file.py


# send-file.py

This script sends an encrypted file from a client to a server using a Python socket. The file is encrypted using the AES encryption algorithm with a specified key.

## Usage

1. Ensure that Python is installed on both the client and server machines.

2. Run the server script on the machine that will receive the file:

   ```bash
   python recv-file.py

3. Run the client script on the machine that will send the file:

   ```bash
    python send_file.py

Note: Adjust the file name, key, and other parameters in the script according to your requirements.

The client will connect to the server, encrypt the specified file, and send it to the server.
