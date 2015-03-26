#include <sys/types.h>
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
#include <string.h>
#include <dirent.h>


#define MAXBUFSIZE 3000

int main (int argc, char * argv[] )
{


	int sock;
	char command[30], filename[30];                      
	struct sockaddr_in sin, remote;     
	unsigned int remote_length;       
	int nbytes, writesize, readsize;                  
	int bufflen;
	char buffer[MAXBUFSIZE];            
	if (argc != 2)
	{
		printf ("USAGE:  <port>\n");
		exit(1);
	}


	bzero(&sin,sizeof(sin));                   
	sin.sin_family = AF_INET;                   
	sin.sin_port = htons(atoi(argv[1]));        
	sin.sin_addr.s_addr = INADDR_ANY;          


	//Causes the system to create a generic socket of type UDP (datagram)
	if((sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP))==-1)
	{
		printf("unable to create socket");
	}


	if (bind(sock, (struct sockaddr *)&sin, sizeof(sin)) < 0)
	{
		printf("unable to bind socket\n");
	}

	printf("successfully socket created\n");


  while(1)
  {

	remote_length = sizeof(remote);
	bzero(command, 30);
	
	if ((nbytes = recvfrom(sock, command, 30, 0, (struct sockaddr *)&remote, &remote_length)) == -1)
	{
		printf("error in recvfrom");
	}
	printf("The client says %s \n", command);

	if(!strcmp(command, "put") || !strcmp(command, "get"))
	{
	bzero(filename, 30);
	nbytes = recvfrom(sock, filename, 30, 0, (struct sockaddr *)&remote, &remote_length);
	printf("file name received %s \n", filename);
	}
	





	if(!strcmp(command, "put"))
	{
	
	FILE *fp;
	fp = fopen(filename, "w+");

	bzero(buffer,sizeof(buffer));
	nbytes = recvfrom(sock, buffer, sizeof(buffer) , 0, (struct sockaddr *)&remote, &remote_length);
	printf("file recv size = %d",nbytes );
	if(nbytes == -1) printf("Error in recvfrom");
	printf("the recv size is %d\n", nbytes);
	writesize = fwrite(buffer, sizeof(char), nbytes, fp);
	fclose(fp);
	
	}

	



	if(!strcmp(command, "get"))
	{
	printf("command get \n");
	int size;
	FILE *fp2;
	printf("opening file %s\n",filename);
	fp2 = fopen(filename, "r");
	fseek (fp2, 0, SEEK_END);
	size=ftell(fp2);
	fseek (fp2, 0, SEEK_SET);
	
	readsize = fread(buffer,sizeof(char), size, fp2);
	printf("file readsize = %d\n",readsize);
	nbytes = sendto(sock, buffer, readsize, 0,(struct sockaddr *)&remote, sizeof(remote));
	if(nbytes == -1) printf("Error in sendto");
	printf("the sent size is %d\n", nbytes);
	printf("file sent \n");
	fclose(fp2);
	
	}

	if(!strcmp(command, "ls"))
	{
		DIR *d;
		struct dirent *dirent;
		d = opendir(".");

		while ((dirent = readdir(d)) != NULL)
		    {
		    nbytes = sendto(sock, dirent->d_name,strlen(dirent->d_name), 0,(struct sockaddr *)&remote, remote_length);
		    if(nbytes == -1) printf("Error in sendto");      
		    }
		closedir(d);
		
		char END[] = {"END"} ;
		nbytes = sendto(sock, END,strlen(END), 0,(struct sockaddr *)&remote, remote_length);
		if(nbytes == -1) printf("Error in sendto");

	}

	if(!strcmp(command, "exit"))
	{
	printf("server exits \n");
	break;
	}
 }
	close(sock);
}
