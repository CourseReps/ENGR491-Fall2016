from socket import *
import serial

ser = serial.Serial('/dev/rfcomm0', 9600)

#test changes to see if git is working


HOST = '192.168.43.96'
PORT = 3820
server_socket = socket(AF_INET, SOCK_STREAM)
server_socket.connect((HOST,PORT))
message = '0'
while message != '|':
    message = ser.readline()
    if message:
        server_socket.sendall(message)
    else:
        break
print ('end')
server_socket.close()
exit()
