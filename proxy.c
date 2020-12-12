#include <stdio.h>
#include <stdlib.h>
#include <string.h>    	
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h> 	
#include <unistd.h> 
#include <pthread.h>   
#include <netdb.h>
#include <errno.h>

#define SERVER_IP       "1.1.1.1"
#define PORT            10059

pthread_t thread_id[15];
pthread_mutex_t lock;
void *Handle_Comm(void *);
char client_message[20000];
int send_size, recv_size, i = 0;
FILE *fp;

int main(int argc , char *argv[])
{
    int server_sock, client_sock;
    struct sockaddr_in server, client;
    socklen_t sock_len = sizeof(struct sockaddr_in);

    /* Initialize mutex */
    if (pthread_mutex_init(&lock, NULL) != 0) { 
        perror("Mutex init has failed"); 
        exit(EXIT_FAILURE);
    }

    /* Zeroing sockaddr_in structs */
    memset(&server, 0, sizeof(server));

    /* Open log file */
    fp = fopen("Proxy.log", "w");
    fseek(fp, 0, SEEK_SET);
    printf("Log file opened.\n");

    /* Create sockets */
    server_sock = socket(AF_INET, SOCK_STREAM, 0);
    client_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (server_sock < 0 || client_sock < 0)
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
    if(bind(server_sock, (struct sockaddr *)&server, sizeof(server)) < 0)
    {
        perror("Bind failed. Error");
        exit(EXIT_FAILURE);
    }
    printf("Bind completed.\n");

    /* Server socket enters listening mode */
    printf("Server listening on port %d\n", PORT);
    listen(server_sock, 15);

    while(1)
    {
        printf("Waiting for incoming connections...\n\n");
        
        client_sock = accept(server_sock, (struct sockaddr *)&client, (socklen_t*)&sock_len);
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
    fclose(fp);
}

void *Handle_Comm(void *sock) 
{ 
    int client_sock = *((int *)sock);
    int serv_recv_size;

    pthread_mutex_lock(&lock);

    /* Receive message from browser client */
    recv_size = recv(client_sock, client_message, 1024, 0);
    printf("Received %d bytes. Msg is:\n\n%s\n\n", recv_size, client_message);

    fwrite(client_message, recv_size, 1, fp);
    fwrite("\n\r", 2, 1, fp);
    printf("Logged message contents to log file.\n");

    /* Determine the request type */
    char request_type[20], resource[200], http[20], host[50];
    sscanf(client_message, "%s %s %s %*s %s", request_type, resource, http, host);

    /* Debugging purposes */
    printf("========== HTTP Message Info ==========\n");
    printf("Request type: %s\n", request_type);
    printf("Path of resource requested: %s\n", resource);
    printf("Host: %s\n", host);
    printf("=======================================\n\n");

    /* If the request is anything other than a GET request, exit the thread */
    if(strcmp(request_type, "GET") != 0)
    {
        printf("Not a GET request. Thread exiting!\n\n");
    }
    else
    {
        char message[1000000];
        
        /*
         * Code adapted from Beej's Guide to Network Programming
         * http://beej.us/guide/bgnet/html/
         */
        struct addrinfo hints, *servinfo;
        int server_sock, status;

        /* Zeroing hints struct */
        memset(&hints, 0, sizeof hints);

        /* Specify IPv4 and TCP socket type */
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        if((status = getaddrinfo(host, "80", &hints, &servinfo)) != 0)
        {
            perror("getaddrinfo() error");
            exit(EXIT_FAILURE);
        }

        /* Create new socket */
        server_sock = socket(servinfo->ai_family, servinfo->ai_socktype, 0);
        if(server_sock < 0)
        {
            perror("[Thread] Error creating socket");
            exit(EXIT_FAILURE);
        }
        printf("[Thread] Socket created successfully\n");

        /* Make a connection to origin server */
        if (connect(server_sock, servinfo->ai_addr, servinfo->ai_addrlen) < 0)
        {
            perror("[Thread] Connect error") ;
            exit(EXIT_FAILURE);
        }
        printf("[Thread] Connected to origin server.\n");

        /* Send message to origin server */
        sprintf(message, "GET %s %s\r\nHost: %s\r\n\r\n", resource, http, host);
        send_size = send(server_sock, message, strlen(message), 0);
        printf("[Thread] Sent %d bytes to server\n", send_size);

        /* Receive message from origin server */
        serv_recv_size = recv(server_sock, message, sizeof(message), MSG_WAITALL);
        printf("[Thread] Received %d bytes from server ", serv_recv_size);
        printf("Message from server is: \n\n%s\n", message);

        /* Send message from origin server back to client */
        send_size = send(client_sock, message, serv_recv_size, MSG_WAITALL);
        printf("[Thread] Sent %d bytes back to browser client.\n\n", send_size);

        freeaddrinfo(servinfo);
    }
    pthread_mutex_unlock(&lock);
    pthread_exit((void *)0);
}
