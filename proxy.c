#include <stdio.h>
#include <stdlib.h>
#include <string.h>    	
#include <sys/socket.h>
#include <arpa/inet.h> 	
#include <unistd.h> 
#include <pthread.h>   
#include <netdb.h>
#include <errno.h>

#define SERVER_IP       "172.25.42.71"
#define PORT            10059

int i = 0;
pthread_t thread_id[15];
void *Handle_Comm(void *);
char client_message[20000];
int send_size, recv_size;

int main(int argc , char *argv[])
{
    int socket_desc, client_sock, read_size;
    struct sockaddr_in server, client;
    socklen_t sock_len = sizeof(struct sockaddr_in);

    /* Zeroing sockaddr_in structs */
    memset(&server, 0, sizeof(server));

    /* Create sockets */
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (socket_desc < 0 || client_sock < 0)
    {
        perror("Error creating socket");
        exit(EXIT_FAILURE);
    }
    printf("Sockets created\n");

    /* Construct server_addr struct */
    server.sin_family = AF_INET;
    server.sin_port = htons(PORT);
    inet_pton(AF_INET, SERVER_IP, &(server.sin_addr)) ;

    /* Binds socket to port number */
    if(bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Bind failed. Error");
        exit(EXIT_FAILURE);
    }
    printf("Bind completed.\n");

    /* Server socket enters listening mode */
    printf("Server listening on port %d\n", PORT);
    listen(socket_desc, 15);

    while(1)
    {
        printf("Waiting for incoming connections...\n\n");
        
        client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&sock_len);
        if (client_sock < 0)
        {
            perror("accept failed");
            exit(EXIT_FAILURE);
        }
        printf("Connection accepted. Creating thread to handle communication\n\n"); 
        pthread_create(&(thread_id[i]), NULL, Handle_Comm, &client_sock);

        i += 1;
        if(i == 15)
            i = 0;
    }    
}

void *Handle_Comm(void *sock) 
{ 
    int client_sock = *((int *)sock);
    int server_sock = socket(AF_INET, SOCK_STREAM, 0);
    recv_size = recv(client_sock, client_message, 1024, 0);
    printf("Received %d bytes. Msg is:\n\n%s\n\n", recv_size, client_message);

    /* Determine the request type */
    char request_type[20], item[200], host[50];
    sscanf(client_message, "%s %s %*s %*s %s", request_type, item, host);

    printf("Request type: %s\n", request_type);
    printf("Path of resource requested: %s\n", item);
    printf("Host: %s\n", host);
    

    /* If the request is anything other than a GET request, exit the thread */
    if(strcmp(request_type, "GET") != 0)
    {
        printf("Not a GET request. Thread exiting!\n\n");
        pthread_exit((void *)2);
    }
    else
    {

        /*
         * TODO:
         * 1. make a connection to origin server using TCP
         * 2. make new GET request and send to origin server (get addr info function?)
         * 3. send back to browser
         */

        //sprintf(client_message, "GET %s HTTP/1.1\r\n\r\n", origin_server);
        //send_size = send(*((int *)socket), client_message, 64, 0);
    }
}
