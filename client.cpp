#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/time.h>
#include <fcntl.h>
#include <algorithm>
#include <string>
// #include "clients_structure.h"
// #include "sockaddr_in.h"
struct sockaddr_in addr;
// functions
void get_message();
void connection(int *client_sock);
void send_message();
int maxfd();

// vars
#define STDIN 0
int client_sock;    
char buf_write[1000]; 
int bytes_recv = 0;
int bytes_send = 0;


int main(int argc, char const *argv[])
{
    if (argc < 2) {return 0;}

    addr.sin_family = AF_INET;
    addr.sin_port = htons(atoi(argv[2])); 
    inet_pton(AF_INET, argv[1], &addr.sin_addr);

    connection(&client_sock);

    while (1)
    {   
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(client_sock, &fds); 
        FD_SET(STDIN, &fds);         
            
        if (select(maxfd(), &fds, NULL, NULL, NULL) <= 0)
        {
            perror("select");
            exit(3);
        }  

        if (FD_ISSET(STDIN, &fds)){
            send_message();
        }

        if (FD_ISSET(client_sock, &fds)){
            std::cout << "server : ";
            get_message();
            std::cout << std::endl;
            if (bytes_recv <= 0) {break;}
            
        }
    }    
    close(client_sock);
    return 0;
}



void connection(int *client_sock) {   
    // socket()
    *client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if(client_sock < 0)
    {
        perror("socket");
        exit(1);
    }

    // connect()
    std::cout << "=> Awaiting confirmation from the server..." << std::endl;   

    if(connect(*client_sock, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("connect");
        exit(2);
    } 

    get_message();
}

void get_message() { 
    bytes_recv = recv(client_sock, buf_write, sizeof(buf_write), 0);
    if (bytes_recv <= 0)  {
        // error("Error on reading");
        close(client_sock);
        std::cout << "connection closed" << std::endl; 
    }   
    char *message = new char[bytes_recv];  
    for (int i = 0; i < bytes_recv; i++)
    {
        message[i] = buf_write[i];
    }
    message[bytes_recv] = '\0';
    std::cout << message;
    delete [] message;
}

void send_message() { 
    std::cin.getline(buf_write, 1000);
    bytes_send = 0;
    for (int i = 0; buf_write[i]; i++)
    {
        bytes_send++;
    }
    char *message = new char[bytes_send];
    for (int i = 0; i < bytes_send; i++)
    {
        message[i] = buf_write[i];                      
    }
    message[bytes_send] == '\0'; 
    send(client_sock, message, bytes_send, 0);
    delete[] message; 
}

int maxfd() {
    return ((client_sock > STDIN) ? client_sock : STDIN) + 1;
}







