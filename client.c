#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/time.h>
#include <stdlib.h>
#include <memory.h>
#include <errno.h>


#define MAXBUFSIZE 3000

int main (int argc, char * argv[])
{

	int nbytes;                            
	int sock, readsize, writesize;                               
	char buffer[MAXBUFSIZE];
	unsigned int remote_length;
	char command[30], filename[30];
	struct sockaddr_in remote;              

	if (argc < 3)
	{
		printf("USAGE:  <server_ip> <server_port>\n");
		exit(1);
	}

	
	bzero(&remote,sizeof(remote));              
	remote.sin_family = AF_INET;                 
	remote.sin_port = htons(atoi(argv[2]));      
	remote.sin_addr.s_addr = inet_addr(argv[1]); 

	//Causes the system to create a generic socket of type UDP (datagram)
	if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
	{
		printf("unable to create socket \n");
	}
	printf("Successfully socket created \n");

		
  while(1)
  {	
	
	printf("Enter command\n ");
	scanf("%s",command);
	if(!strcmp(command, "put") || !strcmp(command, "get"))
	{
	printf("Enter filename \n");
	scanf("%s", filename);
	}
	


	remote_length = sizeof(remote);

	nbytes = sendto(sock, command, 30, 0,(struct sockaddr *)&remote, sizeof(remote));
	printf("the sent command is %s \n", command);

	if(!strcmp(command, "put") || !strcmp(command, "get"))
	{
	nbytes = sendto(sock, filename, 30, 0,(struct sockaddr *)&remote, sizeof(remote));
	printf("the sent filename is %s \n", filename);

	}





	if(!strcmp(command, "put"))
	{
	printf("command put \n");
	int size2;
	FILE *fp2;
	fp2 = fopen(filename, "r");
	
	fseek (fp2, 0, SEEK_END);
	size2=ftell(fp2);
	fseek (fp2, 0, SEEK_SET);	

	bzero(buffer,sizeof(buffer));
	readsize = fread(buffer,sizeof(char), size2, fp2);
	printf("file readsize = %d\n",readsize );	
	nbytes = sendto(sock, buffer, readsize, 0, (struct sockaddr *)&remote, sizeof(remote));
	if(nbytes == -1) printf("Error in sendto \n");
	printf("file size = %d\n",nbytes );
	printf("File sent \n");	
	fclose(fp2);
	
	}




	if(!strcmp(command, "get"))
	{
	printf("command get \n");
	int size;
	FILE *fp;
	fp = fopen(filename,"w+");

		bzero(buffer,sizeof(buffer));
		nbytes = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&remote, &remote_length);
		if(nbytes == -1) printf("Error in recvfrom \n");
		printf("file size = %d",nbytes );
		printf("the recv size is %d\n", nbytes);
		writesize = fwrite(buffer, sizeof(char), nbytes, fp);
		printf("file received \n");
		fclose(fp);
		
	}




	if(!strcmp(command, "ls"))
	{
	bzero(buffer,sizeof(buffer));
	while(1)
	{

	nbytes = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr *)&remote, &remote_length);
	if(!strcmp(buffer, "END")) break;
	printf("%s \n",buffer);
	bzero(buffer,sizeof(buffer));
	}

	}




	if(!strcmp(command, "exit"))
	{
	printf ("client is exiting now\n"); 
	break;
	}




	if(strcmp(command, "exit") && strcmp(command, "put") && strcmp(command, "get") && strcmp(command, "ls")) 
	printf("Invalid command \n");


  }
	printf("Socket closed \n");
	close(sock);

}
