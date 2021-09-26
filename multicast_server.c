/* Send Multicast Datagram code example. */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

struct in_addr localInterface;
struct sockaddr_in groupSock;
int sd;
char databuf[1024] = "Multicast test message lol!";
int datalen = sizeof(databuf);
 
int main (int argc, char *argv[ ])
{
/* Create a datagram socket on which to send. */
	sd = socket(AF_INET, SOCK_DGRAM, 0);
	FILE *fp = fopen("test.txt","r");
	struct stat st;
	stat("test.txt", &st);
	double size = st.st_size;

	if(sd < 0)
	{
	  perror("Opening datagram socket error");
	  exit(1);
	}
	else
	  printf("Opening the datagram socket...OK.\n");
	 
	/* Initialize the group sockaddr structure with a */
	/* group address of 226.1.1.1 and port 4321. */
	memset((char *) &groupSock, 0, sizeof(groupSock));
	groupSock.sin_family = AF_INET;
	groupSock.sin_addr.s_addr = inet_addr("226.1.1.1");
	groupSock.sin_port = htons(4321);
	 
	/* Set local interface for outbound multicast datagrams. */
	/* The IP address specified must be associated with a local, */
	/* multicast capable interface. */
	localInterface.s_addr = inet_addr("10.0.2.15");
	
	/* IP_MULTICAST_IF:  Sets the interface over which outgoing multicast datagrams are sent. */
	if(setsockopt(sd, IPPROTO_IP, IP_MULTICAST_IF, (char *)&localInterface, sizeof(localInterface)) < 0)
	{
	  perror("Setting local interface error");
	  exit(1);
	}
	else
	  printf("Setting the local interface...OK\n");
	/* Send a message to the multicast group specified by the*/
	/* groupSock sockaddr structure. */
	/*int datalen = 1024;*/
	
	while(!feof(fp)){
		fread(databuf, sizeof(char), sizeof(databuf), fp);
		
		if(sendto(sd, databuf, datalen, 0, (struct sockaddr*)&groupSock, sizeof(groupSock)) < 0)
		{
			perror("Sending datagram message error");
		}
	}
	printf("Sending datagram message...OK\n");

	memset(databuf, 0, sizeof(databuf));
	databuf[0] = 'e';
	databuf[1] = 'n';
	databuf[2] = 'd';
	databuf[3] = 'o';
	databuf[4] = 'f';
	databuf[5] = 'i';
	databuf[6] = 'n';
	databuf[7] = 'p';
	databuf[8] = 'u';
	databuf[9] = 't';
	
	sendto(sd, databuf, datalen, 0, (struct sockaddr*)&groupSock, sizeof(groupSock));
	printf("file size : %f MB\n",size/1000000);
	close(sd);
	fclose(fp);
	/* Try the re-read from the socket if the loopback is not disable
	if(read(sd, databuf, datalen) < 0)
	{
	perror("Reading datagram message error\n");
	close(sd);
	exit(1);
	}
	else
	{
	printf("Reading datagram message from client...OK\n");
	printf("The message is: %s\n", databuf);
	}
	*/
	return 0;
}
