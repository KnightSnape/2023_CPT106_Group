import socket

client_socket = socket.socket(socket.AF_INET)

host = 'localhost'
port = 8080

client_socket.connect((host,port))

# data = '{"auth":{"account":"cpt106","password":"1919810"},"operation":{"arguments":{ }, "name": "ping" } }'

data = '{"auth":{"account":"cpt106","password":"35086020"},"operation":{"arguments":{"plate_number":"123456","type":"car"},"name":"vehicle_enter"} }'

client_socket.sendall(data.encode('ascii'))
print('Send data: {}'.format(data))

response = client_socket.recv(1024)

print('Receive data: {}'.format(response.decode('ascii')))

client_socket.close()