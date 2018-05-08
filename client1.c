#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>

#define MAXLINE 4096   /*max text line length*/
#define SERV_PORT 3000 /*port*/

int main(int argc, char **argv)
{
	int sockfd;
	struct sockaddr_in servaddr;
	char sendline[MAXLINE], recvline[MAXLINE];
	FILE *fp;

	/* Open a log file to append*/
	fp = fopen("a4p3Client1Log.txt", "a");

	//  Create a socket for the client
	//  If sockfd<0 there was an error in the creation of the socket
	if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) <0)
	{
		perror("Problem in creating the socket");
		exit(2);
	}

	//  Creation of the socket
	memset(&servaddr, 0, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	// taking the port and IP address from user
	servaddr.sin_addr.s_addr = inet_addr(argv[1]);
	servaddr.sin_port = htons((int) strtol(argv[2], (char **)NULL, 10));

	// Connection of the client to the socket
	if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr))<0)
	{
		perror("Problem in connecting to the server");
		exit(3);
	}
	else
	{
		printf("Connection established. Sending request to the server.\n");
	}

	// send request to server
	printf("Request:  ");
	strcpy(sendline, "SELECT * FROM EMPLOYEE;");

	while (1)
	{
		// open the file for append
		fp = fopen("a4p3Client1Log.txt", "a");

		printf("%s\n", sendline);
		send(sockfd, (const void*)sendline, strlen(sendline), 0);

		if (recv(sockfd, recvline, MAXLINE,0) == 0)
		{
			//error: server terminated prematurely
			perror("The server terminated prematurely");
			exit(4);
		}

		// print out to console
//		printf("%s", "Result received from the server: \n");
//		fputs(recvline, stdout);

		// append to the file
		fprintf(fp, "Result received from the server: \n");
		int result = fputs(recvline, fp);

		if (result == EOF)
		{
			printf("Cannot write\n");
		}

		fclose(fp);
		// go to sleep for 3 seconds
		sleep(3);

		// start the loop again
		printf("Request:  ");
	}

	fclose(fp);
}