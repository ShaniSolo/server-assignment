#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <errno.h>





int main(int argc, char *argv[])
{
	if (argc != 4)
	{
        fprintf(stderr, "%s\n", strerror(errno));
        exit(1);

    }
	char* ip = argv[1];
	uint16_t port = atoi(argv[2]);
	char* path = argv[3];
	
	int sockfd = -1;
	char recv_buff[1024];
	struct sockaddr_in serv_addr; 
	unsigned int numOfBytes = 0;
	unsigned int numOfPrintable = 0;
	char charFromFile;
	FILE* file = fopen(path, "r");	
	
	memset(recv_buff, 0, sizeof(recv_buff));
	if( (sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Error : Could not create socket \n");
		return 1;
	}
	memset(&serv_addr, 0, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(port); 
	serv_addr.sin_addr.s_addr = inet_addr(ip); 	
	

	if(connect(sockfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\n Error : Connect Failed. %s \n", strerror(errno));
		return 1;
	}
	
	if(file == NULL)
	{
		printf("\n Error : Open file Failed. %s \n", strerror(errno));
		return 1;
	}
	while( charFromFile!=EOF)
	{
		charFromFile=fgetc(file);
		numOfBytes++;
	}
	if(write(sockfd, &numOfBytes, sizeof(numOfBytes)) < 0)
	{
		printf("\n Error : Write to socket Failed. %s \n", strerror(errno));
		return 1;
	}
	rewind(file);
	for(int i=0; i < numOfBytes; i++) 
	{
		char charFromFile = fgetc(file);
		if(write(sockfd, &charFromFile, 1) < 0)
		{
			printf("\n Error : Write to socket Failed. %s \n", strerror(errno));
			return 1;
		}
	
	}
	fclose(file);
	if( read(sockfd, &numOfPrintable, sizeof(unsigned int))< 0 )
	{
		  printf("\n Error : Read fron socket Failed. %s \n", strerror(errno));
	 	 return 1;
	}
	
	close(sockfd); 
	printf("# of printable characters: %u\n", ntohl(numOfPrintable));
	return 0;
}