#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <pthread.h>
#include <ctype.h>

// udp skeleton implementation referenced from https://gist.github.com/karupanerura/00c8ff6a48d98dd6bec2

//Structure for udp socket
struct udp_socket
{
    int fd;
    struct sockaddr_in addr;
};

//Initializes socket
struct sockaddr_in create_sockaddr_in(const char *target, const int port)
{
    struct sockaddr_in addr = {.sin_family = AF_INET};
    addr.sin_port = htons(port);
    addr.sin_addr.s_addr = inet_addr(target);
    return addr;
}

//connects to server
struct udp_socket connect_udp(const char *target, const int port)
{
    struct udp_socket sock;
    sock.fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock.fd < 0)
    {
        perror("sock");
        return sock;
    }
    sock.addr = create_sockaddr_in(target, port);

    static const int nonblocking = 1;
    ioctl(sock.fd, FIONBIO, &nonblocking);

    return sock;
}

//For sending to server
int send_by_udp(const struct udp_socket sock, char *msg, size_t length, size_t offset)
{
    return sendto(sock.fd, msg, length, offset, (struct sockaddr *)&sock.addr, sizeof(sock.addr));
}

//For receiving from server
int recv_by_udp(const struct udp_socket sock, char *buf, size_t length, size_t offset)
{
    socklen_t addrlen = sizeof(sock.addr);
    return recvfrom(sock.fd, buf, length, offset, (struct sockaddr *)&sock.addr, &addrlen);
}

int hasLetter(char* string){
    for(int i = 0; i < strlen(string); i++){
        if (isalpha(string[i]) ){
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{   

    //read IP and Port from input
    int PORT = atoi(argv[2]);

    //constructs socket, prints error if any
    const struct udp_socket sock = connect_udp(argv[1], PORT);
    if (sock.fd < 0)
    {
        perror("connection failure");
        return -1;
    }

    char buf[4096];

    printf("Enter string: ");
    fgets(buf, 4096, stdin);

    //while number is greater than one digit, send back to server
    while (strlen(buf) > 1)
    {
        int length = 0;

        send_by_udp(sock, buf, strlen(buf), 0);
        while ((length = recv_by_udp(sock, buf, 4096, 0)) < 0);

        if (hasLetter(buf)){
            printf("From server: Sorry, cannot compute!\n");
        }else{
            printf("From server: %.*s\n", length, buf);
        }
        

    }
    //close socket
    close(sock.fd);
    return 0;
}
