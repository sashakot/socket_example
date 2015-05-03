/* A simple server in the internet domain using TCP
 *    The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 

void error(const char *msg)
{
	perror(msg);
	exit(1);
}

void run_client(struct hostent *server, int portno)
{
	int sockfd, n, rt;
	struct sockaddr_in serv_addr;
	char buffer[256];

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, 
			(char *)&serv_addr.sin_addr.s_addr,
			server->h_length);
	serv_addr.sin_port = htons(portno);
	rt = connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr));
	if (rt < 0)
		error("ERROR connecting");
	printf("Please enter the message: ");
	bzero(buffer,256);
	fgets(buffer,255,stdin);
	while (1) {
		n = write(sockfd,buffer,strlen(buffer));
		if (n < 0) 
			error("ERROR writing to socket");
		bzero(buffer,256);
		n = read(sockfd,buffer,255);
		if (n < 0) 
			error("ERROR reading from socket");
		printf("%s\n",buffer);
	}
	close(sockfd);
}

void run_server(int portno)
{
	int sockfd, newsockfd;
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int n, rt;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0) 
		error("ERROR opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portno);
	rt = bind(sockfd, (struct sockaddr *) &serv_addr,
				sizeof(serv_addr));
	if (rt < 0)
		error("ERROR on binding");
	listen(sockfd,5);
	clilen = sizeof(cli_addr);
	while (1) {
		newsockfd = accept(sockfd, 
				(struct sockaddr *) &cli_addr, 
				&clilen);
		if (newsockfd < 0) 
			error("ERROR on accept");
		bzero(buffer,256);
		n = read(newsockfd,buffer,255);
		if (n < 0) error("ERROR reading from socket");
		printf("Here is the message: %s\n",buffer);
		n = write(newsockfd,"I got your message",18);
		if (n < 0) error("ERROR writing to socket");
		close(newsockfd);
	}
	close(sockfd);
}

int main(int argc, char *argv[])
{
	int is_server, portno;
	struct hostent *server;

	if (argc < 2) {
		fprintf(stderr,"ERROR, no port provided\n");
		fprintf(stderr,"usage: server : %s port\n", argv[0]);
		fprintf(stderr,"usage: client : %s port hostname\n", argv[0]);
		exit(1);
	}

	is_server = argc < 3 ? 1: 0;
	portno = atoi(argv[1]);

	if (!is_server) {
		server = gethostbyname(argv[2]);
		if (server == NULL) {
			fprintf(stderr,"ERROR, no such host\n");
			exit(0);
		}
	}

	is_server ? run_server(portno): run_client(server, portno); 

	return 0; 
}
