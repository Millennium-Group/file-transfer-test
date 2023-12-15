import socket
import time
from Crypto.Cipher import AES


def closesocket():
    print('closing!')
    client.close()
    exit()

#send file
def sending():
    filename = "txfile"
    file = open(filename, 'rb')  
    # Open file in binary mode
    buf = 1000  
    # Buffer size

    while (True):
        binarydata = file.read(buf)
        if len(binarydata) < 1: closesocket()
        cipher = AES.new(key, AES.MODE_EAX)
        nonce = cipher.nonce
        ciphertext, password = cipher.encrypt_and_digest(binarydata)
        ciphertext = ciphertext + b'unique2023!' + password + b'unique2023!' + nonce
        #time to switch context
        time.sleep(0.00)
        client.sendto(ciphertext, server_address)

#create a UDP socket connection
client = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#bind the socket to the port
server_address = (socket.gethostname(), 10000)
#create 128 bit key for AES encryption
key = b'1234432156788765'
sending()
