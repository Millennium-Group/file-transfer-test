How to setup the project.

1->Create a virtual environment for the project and install the requirements using the following command
    `pip install -r requirements.txt`

2->Now we need to setup the credentials for the ssl to transfer file more securely. Follow the below steps
    # Generate a private key
    # openssl genpkey -algorithm RSA -out server.key

    # # Generate a certificate signing request (CSR) 
    # openssl req -new -key server.key -out server.csr
    After entering the command follow the instructions in the terminal.

    # # Generate a self-signed certificate using the CSR
    # openssl x509 -req -days 365 -in server.csr -signkey server.key -out server.crt

3-> Open a terminal and enter the command `python server_side.py` 
    This will create a socket connection you will see that in the terminal.

4-> Open a new terminal and enter the command `python client_side.py`
    For this to work keep a file named `file_to_transfer.txt` in the root directory.

5-> Now there will be a new file created in the root directory with the name `received_file.txt`

Please make sure if it throws an error regarding `invalid literal for int() with base 10` please re-run the program a few more times from the client side.

##This code covers the following cases:
-> A server connection is established then the client sends a key and a file to the server and it is encrypted before sending.
-> The key is shared using the socket.
-> There is ssl in the process for security
-> The decryption is included because to check if the file is encrypted succesfully and there is no data loss

## Areas of improvement
-> The memory can be managed in a more efficient way
-> User should be able to upload any file from the system.
-> Decryption should be included in another file which only focuses on the decryption.