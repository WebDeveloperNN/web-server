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

char buffer[1024];

int main(int argc, char const *argv[])
{
    int sock_client;
    struct sockaddr_in client_addr;

    sock_client = socket(AF_INET, SOCK_STREAM, 0);
    if(sock_client < 0)
    {
        // perror("socket");
        // exit(1);
    }



    client_addr.sin_family = AF_INET;
    client_addr.sin_port = htons(atoi(argv[2])); 
    inet_pton(AF_INET, argv[1], &client_addr.sin_addr);

    // инициировать соединение на сокете
    // Системный вызов connect () соединяет сокет, указанный дескриптором файла listen_socket_onServer, с адресом, указанным в server_addr. Аргумент sizeof server_addr указывает размер addr.  
    std::cout << "=> Awaiting confirmation from the server..." << std::endl;   
    if(connect(sock_client, (struct sockaddr *)&client_addr, sizeof(client_addr)) < 0)
    {
        perror("connect");
        exit(2);
    }
    if (recv(sock_client, buffer, sizeof(buffer), 0) < 0)
    {
        // error("Error on reading");
    }

    fcntl(sock_client, F_SETFL, O_NONBLOCK);

    while (1)
    {








        // fd_set readset; 
        // FD_ZERO(&readset);
        // FD_SET(sock_client, &readset);

        // fd_set writefds; 
        // FD_ZERO(&writefds);
        // FD_SET(sock_client, &writefds);

        // timeval timeout;
        // timeout.tv_sec = 5;
        // timeout.tv_usec = 0;

        // if(select(sock_client+1, &readset, &writefds, NULL, &timeout) <= 0)
        // {
        //     perror("select");
        //     exit(3);
        // }

        // if(FD_ISSET(sock_client, &writefds))
        // {
        //     std::cout << "> ";
        //     std::cin.getline(buffer, sizeof(buffer));
        //     send(sock_client, buffer, sizeof(buffer), 0);
        // }

        // if(FD_ISSET(sock_client, &readset))
        // {
        //     recv(sock_client, buffer, sizeof(buffer), 0);
        //     printf("server : %s\n", buffer); 
        // }





        //select time = NULL






        //working
        std::cout << "> ";
        std::cin.getline(buffer, sizeof(buffer));
        if (send(sock_client, buffer, sizeof(buffer), 0) < 0)
        {
            // error("Error on writting");
        }   

        if (recv(sock_client, buffer, sizeof(buffer), 0) == 0)
        {
                // error("Error on reading");
        }
        printf("server : %s\n", buffer);  
        
        if (int i = strncmp("bye", buffer, 3) == 0)
        {
            break;
        }
        
        
    }

    close(sock_client);

    return 0;
}
    



