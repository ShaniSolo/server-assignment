#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include <assert.h>
#include <ctype.h>
#include <signal.h>
#include <pthread.h>



pthread_mutex_t qlock;
unsigned int pcc_total[95]= {0};

void sigHndlr() {
	pthread_mutex_lock(&qlock);
	for(int i = 0; i < 95; i++)
	{
		printf("char ’%c’ : %u times\n", (i+32), pcc_total[i]);
	    
	}
	pthread_mutex_unlock(&qlock);
	exit (0);
}

int main(int argc, char *argv[])
{
    if (argc != 2) {
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);
    }
	if (signal(SIGINT, &sigHndlr) == SIG_ERR) {
        fprintf(stderr, "%s", strerror(errno));
        exit(1);
    }

	int listenfd = -1;
	int connfd = -1;
	int printablesChar =  0;
	int portAddress = atoi(argv[1]);
	unsigned int readChars =  0;

	
	struct sockaddr_in serv_addr;
	struct sockaddr_in peer_addr;
	socklen_t addrsize = sizeof(struct sockaddr_in );
	
	
	listenfd = socket( AF_INET, SOCK_STREAM, 0 );
	memset( &serv_addr, 0, addrsize );
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
	serv_addr.sin_port = htons(portAddress);
	if( 0 != bind( listenfd,(struct sockaddr*) &serv_addr, addrsize ) )
	{
		printf("\n Error : Bind Failed. %s \n", strerror(errno));
		return 1;
	}
	if( 0 != listen( listenfd, 10 ))
	{
		printf("\n Error : Listen Failed. %s \n", strerror(errno));
		return 1;
	}
	while( 1 )
	{
		connfd = accept( listenfd,(struct sockaddr*) &peer_addr, &addrsize);
		if( connfd < 0 )
		{
		  printf("\n Error : Accept Failed. %s \n", strerror(errno));
		  return 1;
		}
		
		if( read(connfd, &readChars, sizeof(readChars))<0 )
		{
		  printf("\n Error : Read from socket Failed. %s \n", strerror(errno));
		  return 1;
		}

		char readChar='a';
		printablesChar = 0;
		while( readChars > 0 )
		{
		 	
			if( read(connfd, &readChar, 1)< 0 )
			{
				  printf("\n Error : Read from socket Failed. %s \n", strerror(errno));
			 	 return 1;
			}
			if(readChar <= 126 && readChar >= 32)
			{
				pcc_total[readChar-32]++;
				printablesChar++;
			}
			readChars--;
		}
		int numToSend;
		numToSend = htonl(printablesChar);
		if(write(connfd, &numToSend, sizeof(unsigned int)) < 0)
		{
			printf("\n Error : Write to socket Failed. %s \n", strerror(errno));
			return 1;
		}
		

	}
	close(connfd);
}