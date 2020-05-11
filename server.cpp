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
// #include "clients_structure.h"
// #include "sockaddr_in.h"

//functions 
void send_message(int *socket, int status = 0);
void get_message(int *socket, int status = 0);
void connection(int *listen_socket);
int maxfd();

// vars
struct sockaddr_in server_addr;
int listen_socket;
char buf_write[1000]; 
int bytes_recv = 0; // = bytes_recv in send_message()
int client_socket;    
std::set<int> clients;
char **temp;


int main(int argc, char const *argv[])
{
    // вынести
    // if argc < 2 то адрес loopback, порт случайный, если больше 2, то код ниже
    if (argc < 2) {return 0;}  
    
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(atoi(argv[2])); 
    inet_pton(AF_INET, argv[1], &server_addr.sin_addr);

    connection(&listen_socket);

    while(1)
    { 
        fd_set readset;    
        FD_ZERO(&readset);
        FD_SET(listen_socket, &readset);

        for(auto it = clients.begin(); it != clients.end(); it++) {
            FD_SET(*it, &readset);
        }    
        
        if(select(maxfd(), &readset, NULL, NULL, NULL) <= 0)
        {
            perror("select"); 
            exit(3);
        }

        if(FD_ISSET(listen_socket, &readset))
        {     
            get_message(&listen_socket, 3);
        }
        

        for(auto it = clients.begin(); it != clients.end(); it++)
        {   
            int  temp = *it;
            if(FD_ISSET(*it, &readset))
            {   
                get_message(&temp);
            }            
            
        }        
    }  
        
    close(listen_socket);

    return 0;
}






// connection
void connection(int *listen_socket) { 

    // socket()
    *listen_socket = socket(AF_INET, SOCK_STREAM, 0);
    if(*listen_socket < 0)
    {
        perror("socket");
        exit(1);
    }
    
    fcntl(*listen_socket, F_SETFL, O_NONBLOCK);

    // bind()
    if(bind(*listen_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
    {
        perror("bind");
        exit(2);
    }

    // listen
    listen(*listen_socket, 10);
}





// get_message
void get_message(int *socket, int status) { 
   
    if (status == 3)
    {
        // connect with the client
        int client_socket = accept(listen_socket, NULL, NULL);
        if(client_socket < 0)
        {
            perror("accept");
            exit(3);
        }
        fcntl(client_socket, F_SETFL, O_NONBLOCK);
        clients.insert(client_socket); 
        send_message(&client_socket, 3);        
        std::cout << "=> Connected with the client" << std::endl;
    } else {
        bytes_recv = recv(*socket, buf_write, sizeof(buf_write), 0);
        if (bytes_recv <= 0)  {
            close(*socket);
            clients.erase(*socket);
            std::cout << "=> Client disconnected" << std::endl;
        }  
        char *message = new char[bytes_recv];
        for (int i = 0; i < bytes_recv; i++)
        {
            message[i] = buf_write[i];                      
        }
        message[bytes_recv] == '\0';
        temp = &message;
        send_message(socket);
        delete [] message;
    }   
}





// send_message
void send_message(int *socket, int status) {
    if (status == 3)
    {
        char server_connected_message[] = "=> Server connected...\n";
        send(*socket, server_connected_message, sizeof(server_connected_message), 0);
    } else {       
        for(auto it = clients.begin(); it != clients.end(); it++)
        {
            send(*it, *temp, bytes_recv, 0);
        }
    }     

}



// maximum
int maxfd() {
    return (std::max(listen_socket, *max_element(clients.begin(), clients.end()))+1);
}



