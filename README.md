# Concurrent Thread
Server - Client using Concurrent Thread

This project implements the concurrent thread to handle multiple clients at the same time.

The server is up and listens up to 5 clients and serves at most 5 Clients at the same time. When a Client is connected, it sends a request to the server asking for information in a database.
The server then open the SQLite database, do query and send back the result to the Client.
Server creates a new thread for each Client connecting.

How to run:
Run the server first with input of port number
gcc server.c -o server -sqlite3 -pthread -std=c99
./server 2000

Run the clients with input of IP address and port number
gcc client1 -o client1 -std=c99
./client 127.0.0.1 2000
