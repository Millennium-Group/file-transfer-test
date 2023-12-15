import socket
from Crypto.Cipher import AES
import time

#recieve file
def receiving(ctext):
    file = open('rxfile', 'wb')
    try:
        while (True):
            ctext, ignore, nonce = ctext.rpartition(b'unique2023!')
            ctext, ignore, password = ctext.rpartition(b'unique2023!')

            cipher = AES.new(key, AES.MODE_EAX, nonce=nonce)
            binarydata = cipher.decrypt(ctext)

            # message verification with password.
            try:
                #verify integrity
                cipher.verify(password)
            except ValueError:
                print("message corrupted")
                print('closing')
                file.close()
                server.close()
                exit()
            file.write(binarydata)

            server.settimeout(2)
            ctext, address = server.recvfrom(buf)

    except socket.timeout:
        print('closing')
        file.close()
        server.close()
        exit()

#create UDP socket
server = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
#bind socket to port
server_address = (socket.gethostname(), 10000)
#bind socket to address
server.bind(server_address)
buf = 32768
#128 bit key
key = b'1234432156788765'
print('waiting for client')
ctext, address = server.recvfrom(buf)
receiving(ctext)

