from socket import *
import sqlite3

#creating the server connection
HOST = '192.168.43.96'
PORT = 3820
server_socket = socket(AF_INET, SOCK_STREAM)
server_socket.bind((HOST, PORT))
server_socket.listen(1)
conn, addr = server_socket.accept()

#creating the database
sqlserv = sqlite3.connect('flow.db')
curse = sqlserv.cursor()

curse.execute('''CREATE TABLE flowrate(flowrate text)''')

message = str(conn.recv(1024),"utf-8")

while len(message) != 0:
    print(message)
    message = str(conn.recv(1024),"utf-8")
    curse.execute("INSERT INTO flowrate VALUES (?)", (message,))
    sqlserv.commit()

sqlserv.close()
server_socket.close()
exit()
