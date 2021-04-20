/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <time.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}


//Delay code inspired from: https://www.geeksforgeeks.org/time-delay-c/
void delay(int number_of_seconds)
{
    // Converting time into milli_seconds
    int milli_seconds = number_of_seconds;

    // Storing start time
    clock_t start_time = clock();

    // looping till required time is not achieved
    while (clock() < start_time + milli_seconds)
        ;
}

char* sumOfDigits(char *bufferCopy){
    int total = 0; 
    for(int i = 0; i<strlen(bufferCopy); i++){
	char currentChar = bufferCopy[i];	
        if( (currentChar >= '0' && currentChar <= '9')){      
            total += (currentChar)-'0';
        }
        else{
           char* result = malloc(2); 
           result[0] = 'a'; 
           result[1] = '\0'; 
           return result; 
        } 
    }
    //converting int to char, with answer from: 
    //https://stackoverflow.com/questions/8257714/how-to-convert-an-int-to-string-in-c
    if(strlen(bufferCopy) > 0){
    int strLength = snprintf(NULL, 0, "%d", total); 
    char* result = malloc(strLength+1); 
    snprintf(result, strLength+1, "%d", total); 
    return result; 
    }
    char* result = malloc(2);
    result[0] = 'a';
    result[1] = '\0';
    return result;
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     char buffer[256];
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) {
         fprintf(stderr,"ERROR, no port provided\n");
         exit(1);
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5); 
     clilen = sizeof(cli_addr);
     while(1){
     newsockfd = accept(sockfd, 
                 (struct sockaddr *) &cli_addr, 
                 &clilen);
     if (newsockfd < 0) 
          error("ERROR on accept");
     bzero(buffer,256);
     n = read(newsockfd,buffer,255); 
     if (n < 0) error("ERROR reading from socket");
     //printf("Here is the message:%s",buffer); 
     //get the sum
     buffer[strlen(buffer)-1] = '\0';
     char* returnMessage = sumOfDigits(buffer); 
     if(returnMessage[0] != 'a'){
        int lengthOfMessage = strlen(returnMessage);   
	delay(2000);
	n = write(newsockfd, returnMessage, lengthOfMessage);
	while(lengthOfMessage > 1){		
	 returnMessage = sumOfDigits(returnMessage);
         lengthOfMessage = strlen(returnMessage);
	 delay(2000);
	 n = write(newsockfd, returnMessage, lengthOfMessage); 
	}
     } else {
       n = write(newsockfd,"Sorry, cannot compute!",22);
     }
     if (n < 0) error("ERROR writing to socket");
     
      //close connection: 
      close(newsockfd);
     }
     //close(newsockfd);
     //close(sockfd);
     return 0; 
 }
