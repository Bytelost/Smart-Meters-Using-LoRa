import socket

# Server Info
HOST = '192.168.4.1'
PORT = 4080

# Create a socket object
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)

# Define the server address and port
server_address = (HOST, PORT)

# Connect to the server
s.connect(server_address)

# Send data
message = 'Hello, Server!'
s.sendall(message.encode())

# Receive response
data = s.recv(1024)
print('Received', repr(data))

# Close the connection
s.close()