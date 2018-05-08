# Concurrent Thread
Server - Client using Concurrent Thread

This project implements the concurrent thread to handle multiple clients at the same time.

The server is up and listens up to 5 clients and serves at most 5 Clients at the same time. When a Client is connected, it sends a request to the server asking for information in a database.
The server then open the SQLite database, do query and send back the result to the Client.
Server creates a new thread for each Client connecting.
