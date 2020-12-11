#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#define SERVER_IP   "130.111.46.105"

int main()
{
    int err;
    char my_buf[16384] ;
    struct sockaddr_in sa ;
    err = sprintf(my_buf, "%s", "GET /~phillip.dickens/pubs.html HTTP/1.1\r\nHost: www.aturing.umcs.maine.edu\r\n\r\n") ;

    int my_sock = socket(AF_INET, SOCK_STREAM, 0) ;
    if(my_sock < 0)
    {
        perror("Error creating sockets");
        exit(1);
    }
    printf("Socket created successfully.\n");

    sa.sin_family = AF_INET ;
    sa.sin_port = htons(80) ;
    inet_pton(AF_INET, SERVER_IP , &(sa.sin_addr)) ;

    int x = connect(my_sock, (struct sockaddr *) & sa, sizeof(sa)) ;
    if (x !=0)
    {
        perror ("Connect error") ;
        exit (1);
    }

    /* Send message */
    err = send (my_sock, my_buf, 4096,  0) ;
    printf("Sent %d bytes to server\n", err) ;

    /* Receive message */
    err = recv(my_sock, my_buf, 8024,0) ;
    printf("Received %d bytes from server!!!\n", err) ;
    printf("GOT THIS FROM Server: \n\n%s\n", my_buf) ;

    return 0;
}
