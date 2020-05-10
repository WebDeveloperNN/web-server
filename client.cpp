#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <arpa/inet.h>
#include <sys/time.h>
#include <fcntl.h>
#include <algorithm>
#include <string>

#define STDIN 0

int main(int argc, char const *argv[])
{
    struct sockaddr_in client_addr;
    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(atoi(argv[2])); 
    inet_pton(AF_INET, argv[1], &client_addr.sin_addr);

    int sock_client;
    sock_client = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_client < 0)
    {
        perror("socket");
        exit(1);
    }

    std::cout << "=> Awaiting confirmation from the server..." << std::endl;   

    if(connect(sock_client, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0)
    {
        perror("connect");
        exit(2);
    }

    char *server_connected = new char[23];     
    if (recv(sock_client, server_connected, 23, 0) < 0)    {
        // error("Error on reading");
    }
    std::cout << server_connected;

    fcntl(sock_client, F_SETFL, O_NONBLOCK);    

    while (1)
    {
        char buf_write[1000];
        int bytes_recv = 0;

        

        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(sock_client, &fds); 
        FD_SET(STDIN, &fds); 

        int maxfd;
        maxfd = (sock_client > STDIN) ? sock_client : STDIN;    
        if (select(maxfd+1, &fds, NULL, NULL, NULL) <= 0)
        {
            perror("select");
            exit(3);
        }  

        if (FD_ISSET(STDIN, &fds)){
            char *buf_send = new char();
            int sizeof_buf_send = 0;

            std::cin.getline(buf_write, 1000);

            for (int i = 0; buf_write[i]; i++)
            {
                buf_send[i] = buf_write[i];
                sizeof_buf_send++;                        
            }  
        
            send(sock_client, buf_send, sizeof_buf_send, 0);

            delete[] buf_send;
            continue;
        }

        if (FD_ISSET(sock_client, &fds)){  

            char *buf_recover = new char();  

            if ((bytes_recv = recv(sock_client, buf_write, 1000, 0)) <= 0)
            {
                close(sock_client);
                std::cout << "Server is closed" << std::endl;   
                break;       
            }     

            for (int i = 0; i < bytes_recv; i++)
            {
                buf_recover[i] = buf_write[i];       
            }   
            buf_recover[bytes_recv] = '\0';
            std::cout << "server : " << buf_recover << std::endl;
            
            delete[] buf_recover; 
            continue;       
        }
    }    
    close(sock_client); 
    return 0;
}