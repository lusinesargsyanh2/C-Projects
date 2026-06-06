# Multi-Client Chat Application (C)

## Description

This project implements a simple chat system using TCP sockets in C.

### Features

* Multi-client support
* Multithreaded server using POSIX threads (`pthread`)
* Client-side event handling using `epoll`
* Message broadcasting to all connected clients
* Automatic client disconnection handling

---

## Architecture

### Server

The server:

1. Creates a TCP socket.
2. Binds to port `8080`.
3. Listens for incoming connections.
4. Creates a new thread for each connected client.
5. Receives messages from clients.
6. Broadcasts messages to all other connected clients.

### Client

The client:

1. Connects to the server.
2. Uses `epoll` to monitor:

   * Standard input (`stdin`)
   * Server socket
3. Sends user messages to the server.
4. Displays messages received from other clients.

---

## Compilation

### Server

```bash
gcc server.c -o server -lpthread
```

### Client

```bash
gcc client.c -o client
```

---

## Running

### Start Server

```bash
./server
```

Expected output:

```text
Server running on PORT: 8080
```

### Start Clients

Open multiple terminals and run:

```bash
./client
```

---

## Example

Client 1:

```text
Hello everyone!
```

Client 2 receives:

```text
-> Hello everyone!
```

---


## Technologies Used

* C
* POSIX Threads (pthread)
* TCP/IP Sockets
* Linux epoll API

---

## Author

Lusine Sargsyan
