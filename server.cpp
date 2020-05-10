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
#include <set>
#include <algorithm>

int main(int argc, char const *argv[])
{
    if (argc < 2) {return 0;}  

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2])); 
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    int listen_socket_onServer = socket(AF_INET, SOCK_STREAM, 0);
    if(listen_socket_onServer < 0)
    {
        perror("socket");
        exit(1);
    }
    
    fcntl(listen_socket_onServer, F_SETFL, O_NONBLOCK);

    if(bind(listen_socket_onServer, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        exit(2);
    }

    listen(listen_socket_onServer, 10);   

    int bytes_read;
    int connection_socket;    
    std::set<int> clients;
    clients.clear();

    while(1)
    { 
        // int sizeof_buf_send = 0;
        int bytes_recv = 0;

        fd_set readset;    
        FD_ZERO(&readset);
        FD_SET(listen_socket_onServer, &readset);

        for(std::set<int>::iterator it = clients.begin(); it != clients.end(); it++) {
            FD_SET(*it, &readset);
        }     

        int mx = std::max(listen_socket_onServer, *max_element(clients.begin(), clients.end()));
        
        if(select(mx+1, &readset, NULL, NULL, NULL) <= 0)
        {
            perror("select"); 
            exit(3);
        }

        if(FD_ISSET(listen_socket_onServer, &readset))
        {     
            char *buf_send = new char();
            char *buf_write = new char[1000]; 
            std::string info_connect = "=> Server connected...\n";
            
            int connection_socket = accept(listen_socket_onServer, NULL, NULL);
            if(connection_socket < 0)
            {
                perror("accept");
                exit(3);
            }
            
            fcntl(connection_socket, F_SETFL, O_NONBLOCK);
            
            clients.insert(connection_socket); 
          
            for (int i = 0; info_connect[i]; i++)
            {
                buf_send[i] = info_connect[i]; 
              
            }
            send(connection_socket, buf_send, 23, 0);

            
            std::cout << "=> Connected with the client" << std::endl;

            delete [] buf_send;
            delete [] buf_write;
        }
        
        for(std::set<int>::iterator it = clients.begin(); it != clients.end(); it++)
        {
            char *buf_write = new char[1000];
            char *buf_send = new char(); 

            if(FD_ISSET(*it, &readset))
            {       
                if((bytes_recv = recv(*it, buf_write, 1000, 0)) <= 0)
                {
                    close(*it);
                    clients.erase(*it);
                    std::cout << "=> Client disconnected" << std::endl;
                    delete [] buf_send;
                    continue;
                }

                for (int i = 0; i < bytes_recv; i++)
                {
                    buf_send[i] = buf_write[i];
                }              
                buf_send[bytes_recv] = buf_write[bytes_recv];

                for(std::set<int>::iterator it_inner = clients.begin(); it_inner != clients.end(); it_inner++) 
                {
                    send(*it_inner, buf_send, bytes_recv, 0);
                }

                delete [] buf_send;
                delete [] buf_write;
            }            
            
        }
        
    }  
    
    
    close(listen_socket_onServer);

    return 0;
}






