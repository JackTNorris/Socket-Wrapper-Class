#include "socket.h"

Socket::Socket(int domain, int type, int protocol)
{
    this->socket_fd = socket(domain, type, protocol);
    this->domain = domain, this->type = type, this->protocol = protocol;
    this->address.sin_family = AF_INET; 
    this->address.sin_addr.s_addr = INADDR_ANY; 
    isClient = false;
    isServer = false;
    opt = 1;
    if(socket_fd == -1)
    {
        cerr << "ERROR STARTING" << endl;
        exit(EXIT_FAILURE);
    }
    
    if (setsockopt(socket_fd, SOL_SOCKET, SO_KEEPALIVE , &opt, sizeof(opt)))
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    address_length = sizeof(address);
}

void Socket::startListening(int port)
{
    isServer = true;
    this->address.sin_port = htons(port);
    if(bind(socket_fd, (sockaddr *)&address, address_length) < 0){cerr << "Error binding to port" << endl; exit(EXIT_FAILURE);}
    if(listen(socket_fd, 3)){cerr << "Couldn't start listening" << endl; exit(EXIT_FAILURE);}
    cout << "Listening on port: " << port << endl;
    if ((connectedSocket_fd = accept(socket_fd, (struct sockaddr *)&address, (socklen_t*)&address_length)) < 0) 
    { 
        cerr << "Error connecting to client";
        exit(EXIT_FAILURE); 
    }
    cout << "Client connected!" << endl; 
}

string Socket::readData()
{
    if(isServer)
    {
        char buffer[1024] = {'\0'};
        recv(connectedSocket_fd, buffer, 1023,  0);
        if(listen(socket_fd, 3)){cerr << "Couldn't listen" << endl; exit(EXIT_FAILURE);}
        return buffer;
    }
    else
    {
        cerr << "You can't listen for connections if you aren't a server!";
        exit(EXIT_FAILURE);
    }
}

void Socket::connectToSocket(const char *address, int port)
{
    if(!isServer)
    {
        isClient = true;
        connectedSocketAddress.sin_family = AF_INET; 
        connectedSocketAddress.sin_port = htons(port); 
        
        // Convert IPv4 and IPv6 addresses from text to binary form 
        if(inet_pton(AF_INET, address, &connectedSocketAddress.sin_addr)<=0)  
        { 
            printf("\nInvalid address/ Address not supported \n"); 
            exit(EXIT_FAILURE); 
        } 
    
        if (connect(socket_fd, (struct sockaddr *)&connectedSocketAddress, sizeof(connectedSocketAddress)) < 0) 
        { 
            printf("\nConnection Failed \n"); 
            exit(EXIT_FAILURE); 
        }

    }
    else
    {
        cerr << "You must be a client to connect to a server";
        exit(EXIT_FAILURE);
    }
    

}

void Socket::sendData(const char *data)
{
    if(isServer)
    {
        send(connectedSocket_fd, data, strlen(data), 0);
    }
    else if(isClient)
    {
        send(socket_fd, data, strlen(data), 0);
    }
    else
    {
        cerr << "You  must first be connected to a socket before sending a message";
    }
    cout << "Sent message!" << endl;
    
}





