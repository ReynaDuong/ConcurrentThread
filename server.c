#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <sqlite3.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>

/*Defining constant and functions*/
#define PORTNUMBER 8080
#define MAXLINE 4096 /*max text line length*/

struct serverParm
{
	int connectionDesc;
};

void *serverThread(void *parmPtr);
void query(void *parmPtr);

/* The function to receive and send back */
void *serverThread(void *parmPtr)
{
#define PARMPTR ((struct serverParm *) parmPtr)
	int recievedMsgLen;
	char messageBuf[1025];      // hold messages from client
	sqlite3 *db;
	char *zErrMsg = 0;
	int rc;
	char *s;                            // to hold the record
	const char* data = "Record: ";
	sqlite3_stmt *statement;

	/* Server thread code to deal with message processing */
	printf("DEBUG: connection made, connectionDesc = %d\n", PARMPTR->connectionDesc);

	if (PARMPTR->connectionDesc < 0)
	{
		printf("Accept failed\n");
		return(0);    /* Exit thread */
	}

	/* Open database */
	rc = sqlite3_open("employee.db", &db);

	if (rc)
	{
		fprintf(stderr, "Can't open database: %s\n", sqlite3_errmsg(db));
		exit(0);
	}
	else
	{
		fprintf(stderr, "Opened database successfully\n");
	}


	/* Receive messages from sender... */
	recievedMsgLen = (int) read(PARMPTR->connectionDesc, messageBuf, sizeof(messageBuf) - 1);
	while ((recievedMsgLen) > 0)
	{
		// recieve the request and print out
		printf("Message: %s \n", messageBuf);

		// access the sql database and print
		// char *query = "SELECT * from EMPLOYEE;";

		if ( sqlite3_prepare(db, messageBuf, -1, &statement, 0) == SQLITE_OK )
		{
			int ctotal = sqlite3_column_count(statement);
			int res = 0;

			while (1)
			{
				res = sqlite3_step(statement);

				if ( res == SQLITE_ROW )
				{
					for ( int i = 0; i < ctotal; i++ )
					{
						s = (char*)sqlite3_column_text(statement, i);

						// send to the client
						if (write(PARMPTR->connectionDesc,(const char*)s, strlen(s)) < 0)
						{
							perror("Server: write error");
						}

						if (write(PARMPTR->connectionDesc," ", 1) < 0)
						{
							perror("Server: write error");
						}
						printf("%s", s);
						printf(" ");
					}

					// client receives and print a \n
					printf("\n");
					if (write(PARMPTR->connectionDesc,"\n", 1) < 0)
					{
						perror("Server: write error");
					}
				}

				if ( res == SQLITE_DONE || res==SQLITE_ERROR)
				{
					// send done to the client
					if (write(PARMPTR->connectionDesc,"done\n", 5) < 0)
					{
						perror("Server: write error");
					}

					printf("done\n");
					break;
				}
			}
		}

		// send back to client
		if (write(PARMPTR->connectionDesc,"DONE!\n", 6) < 0)
		{
			perror("Server: write error");
			return(0);
		}

		recievedMsgLen = (int) read(PARMPTR->connectionDesc, messageBuf, sizeof(messageBuf) - 1);
	}

	sqlite3_close(db);

	close(PARMPTR->connectionDesc);  /* Avoid descriptor leaks */
	free(PARMPTR);                   /* And memory leaks */
	return(0);                       /* Exit thread */
}


int main (int argc, char **argv)
{
    int listenDesc;
    struct sockaddr_in myAddr;
    struct serverParm *parmPtr;
    int connectionDesc;
    pthread_t threadID;

    char buff[20];
    struct tm *sTm;
    time_t now = time(0);
    sTm = gmtime(&now);
	FILE *fp;

    /* Open a log file to append*/
	fp = fopen("a4p3ServerLog.txt", "a");

    /* Create socket from which to read */
	printf("Creating a socket\n");
    if ((listenDesc = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        perror("open error on socket");
        exit(1);
    }

    /* Create "name" of socket */
    myAddr.sin_family = AF_INET;
    myAddr.sin_addr.s_addr = INADDR_ANY;
    myAddr.sin_port = htons(PORTNUMBER);

	printf("IP Address is : %s\n", inet_ntoa(myAddr.sin_addr));

	//myAddr.sin_port = htons((int) strtol(argv[1], (char **)NULL, 10));

    // Bind
    if (bind(listenDesc, (struct sockaddr *) &myAddr, sizeof(myAddr)) < 0)
    {
        perror("bind error\n");
        exit(1);
    }

    /* Start accepting connections.... */
    /* Up to 5 requests for connections can be queued... */
    listen(listenDesc, 5);

    while (1)
    {
	    /* Wait for a client connection */
	    connectionDesc = accept(listenDesc, NULL, NULL);

	    /* Create a thread to actually handle this client */
	    parmPtr = (struct serverParm *)malloc(sizeof(struct serverParm));
	    parmPtr->connectionDesc = connectionDesc;

	    if (pthread_create(&threadID, NULL, (void *) serverThread, (void *) parmPtr) != 0)
	    {
		    perror("Thread create error\n");
		    close(connectionDesc);
		    close(listenDesc);
		    exit(1);
	    }
	    else
	    {
			fp = fopen("a4p3ServerLog.txt", "a");
		    // print the date and time
		    strftime(buff, sizeof(buff), "%Y-%m-%d %H:%M:%S\n", sTm);
		    fputs(buff, fp);

		    // process ID
		    fprintf(fp, "Process ID : %d\n", getpid());

		    // thread ID
		    fprintf(fp, "Thread ID : %p\n", pthread_self());

		    fprintf(fp, "This thread is created to handled a Client's request \n");
			fclose(fp);
	    }

	    printf("Ready for another client\n");
    }

    // close the file when ends
    fclose(fp);
}

