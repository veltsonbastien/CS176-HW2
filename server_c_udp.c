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

//Begin listening
struct udp_socket listen_udp(const char *target, const int port)
{
    struct udp_socket sock;
    sock.fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock.fd < 0)
    {
        perror("sock");
        return sock;
    }

    const struct sockaddr_in addr = create_sockaddr_in(target, port);
    bind(sock.fd, (struct sockaddr *)&addr, sizeof(addr));

    static const int nonblocking = 1;
    ioctl(sock.fd, FIONBIO, &nonblocking);

    return sock;
}

//For sending to clients
int send_by_udp(const struct udp_socket *sock, char *msg, size_t length, size_t offset)
{
    return sendto(sock->fd, msg, length, offset, (struct sockaddr *)&sock->addr, sizeof(sock->addr));
}

//For receiving from clients
int recv_by_udp(const struct udp_socket *sock, char *buf, size_t length, size_t offset)
{
    socklen_t addrlen = sizeof(sock->addr);
    return recvfrom(sock->fd, buf, length, offset, (struct sockaddr *)&sock->addr, &addrlen);
}

//function taken from https://www.geeksforgeeks.org/how-to-convert-given-number-to-a-character-array/
//For converting integers to characters
char *convertIntegerToChar(int N)
{

    // Count digits in number N
    int m = N;
    int digit = 0;
    while (m)
    {

        // Increment number of digits
        digit++;

        // Truncate the last
        // digit from the number
        m /= 10;
    }

    // Declare char array for result
    char *arr;

    // Declare duplicate char array
    char arr1[digit];

    // Memory allocaton of array
    arr = (char *)malloc(digit);

    // Separating integer into digits and
    // accomodate it to character array
    int index = 0;
    while (N)
    {

        // Separate last digit from
        // the number and add ASCII
        // value of character '0' is 48
        arr1[++index] = N % 10 + '0';

        // Truncate the last
        // digit from the number
        N /= 10;
    }

    // Reverse the array for result
    int i;
    for (i = 0; i < index; i++)
    {
        arr[i] = arr1[index - i];
    }

    // Char array truncate by null
    arr[i] = '\0';

    // Return char array
    return (char *)arr;
}

//sums up digits in string character by character
int addDigits(char *num)
{
    int sum = 0;
    for (int i = 0; num[i] != '\0'; i++)
    {
        int digit = atoi(&num[i]);
        sum += digit;
    }
    return sum;
}

int main(int argc, char *argv[])
{
    //read port input
    int PORT = atoi(argv[1]);
    char *IP = "127.0.0.1";

    //constructs socket, prints error if any
    const struct udp_socket sock = listen_udp(IP, PORT);
    if (sock.fd < 0)
    {
        perror("connection failure");
        return -1;
    }

    int loop = 1;
    while (loop)
    {
        int offset = 0;
        int length = 0;
        char buf[4096];

        //while message contains something
        while ((length = recv_by_udp(&sock, buf, 128, offset)) > 0)
        {
            //calculate sum
            int sum = 0;
            for (int count = 0; count < strlen(buf); count++)
            {
                char c = buf[count];
                int digit = atoi(&c);
                sum += digit;
            }

            char *arr = convertIntegerToChar(sum);

            //send value and clear buffer for new value
            send_by_udp(&sock, arr, length, 0);
            memset(buf, 0, 4096);
            offset += length;
        }
    }

    //close socket
    close(sock.fd);
    return 0;
}
