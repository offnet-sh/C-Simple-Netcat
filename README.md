# C Simple Netcat

A basic Netcat clone written in C.  
Supports both client and server modes for simple TCP communication.

## Features
- Server mode: listens on a specified port, accepts one connection  
- Client mode: connects to a given IP and port  
- Bidirectional communication (send and receive messages)  
- Uses pthreads for simultaneous send/receive  

## Requirements
- Linux/macOS with GCC and pthread support  

## Build

```bash
gcc simple_netcat.c -pthread -o simple_netcat
