#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <errno.h>
#include <time.h>

void error(const char *msg)
{
    perror(msg);
    exit(1);
}

void tcp_server(char *ip, int port, char *file_name);
void tcp_client(char *ip, int port);
void udp_server(char *ip, int port);
void udp_client(char *ip, int port, char *file_name);
void day();

int main(int argc, char *argv[])
{
    if (strcmp(argv[1], "tcp") == 0)
    {
        if (strcmp(argv[2], "send") == 0)
        {
            tcp_server(argv[3], atoi(argv[4]), argv[5]);
        }
        else if (strcmp(argv[2], "recv") == 0)
        {
            tcp_client(argv[3], atoi(argv[4]));
        }
    }
    else if (strcmp(argv[1], "udp") == 0)
    {
        if (strcmp(argv[2], "send") == 0)
        {
            udp_client(argv[3], atoi(argv[4]), argv[5]);
        }
        else if (strcmp(argv[2], "recv") == 0)
        {
            udp_server(argv[3], atoi(argv[4]));
        }
    }
}

void tcp_server(char *ip, int port, char *file_name)
{
    int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[256];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    FILE *fp;
    fp = fopen(file_name, "r");

	struct stat st;
	stat(file_name, &st);
	double size = st.st_size;

	int count = 0;
	int count_per = size/256/4;

	clock_t begin = clock();
	clock_t beginn = begin;
	clock_t end;
	double time_spent;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = inet_addr(ip);
    serv_addr.sin_port = htons(port);
    if (bind(sockfd, (struct sockaddr *)&serv_addr,
             sizeof(serv_addr)) < 0)
        error("ERROR on binding");
    listen(sockfd, 5);
    clilen = sizeof(cli_addr);
    newsockfd = accept(sockfd,
                       (struct sockaddr *)&cli_addr,
                       &clilen);
    if (newsockfd < 0)
        error("ERROR on accept");

    while (!feof(fp))
    {
		count++;
        bzero(buffer, 256);
        n = fread(buffer, sizeof(char), sizeof(buffer), fp);
        /*read file*/
        n = write(newsockfd, buffer, n);
        if (n < 0)
            error("ERROR writing to socket");
    	
		if(count == 1){
			end = clock();
			time_spent = (double)(end-begin)/CLOCKS_PER_SEC;
			printf("0%% ");
			day();
		}else if(count == count_per){
			begin = end;
			end = clock();
			time_spent = (double)(end-begin)/CLOCKS_PER_SEC;
			printf("25%% ");
			day();
		}else if(count == count_per*2){
			begin = end;
			end = clock();
			time_spent = (double)(end-begin)/CLOCKS_PER_SEC;
			printf("50%% ");
			day();
		}else if(count == count_per*3){
			begin = end;
			end = clock();
			time_spent = (double)(end-begin)/CLOCKS_PER_SEC;
			printf("75%% ");
			day();
		}
	}


	begin = end;
	end = clock();
	time_spent = (double)(end-begin)/CLOCKS_PER_SEC;
	printf("100%% ");
	day();
	time_spent = (double)(end-beginn)/CLOCKS_PER_SEC;
	printf("Total trans time: %f ms\n",time_spent*1000);
	printf("file size : %f MB\n",size/1000000);

    close(newsockfd);
    close(sockfd);
    fclose(fp);
    return;
}

void tcp_client(char *ip, int port)
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    FILE *fp;
    fp = fopen("test_input.txt", "w");

    char buffer[256];

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");
    server = gethostbyname(ip);
    if (server == NULL)
    {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }
    bzero((char *)&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr,
          (char *)&serv_addr.sin_addr.s_addr,
          server->h_length);
    serv_addr.sin_port = htons(port);
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    while (1)
    {
        bzero(buffer, 256);
        n = read(sockfd, buffer, 256);
        if (n < 0)
            error("ERROR reading from socket");
        else if (n > 0)
            fwrite(buffer, sizeof(char), n, fp);
        else if (n == 0)
            break;
    }

    close(sockfd);
    fclose(fp);
    return;
}

void udp_server(char *ip, int port)
{
    int sock;
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        error("socket error");

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(ip);

    if (bind(sock, (struct sockaddr *)&servaddr, sizeof(servaddr)) < 0)
        error("bind error");

    char recvbuf[1024] = {0};
    struct sockaddr_in peeraddr;
    socklen_t peerlen;
    int n;
    FILE *fp;
    fp = fopen("test_input.txt", "w");

    while (1)
    {

        peerlen = sizeof(peeraddr);
        memset(recvbuf, 0, sizeof(recvbuf));

        n = recvfrom(sock, recvbuf, sizeof(recvbuf), 0,
                     (struct sockaddr *)&peeraddr, &peerlen);

        if (n == -1)
        {

            if (errno == EINTR)
                continue;

            error("recvfrom error");
        }
        else if (n == 0)
        {
            break;
        }
        else if (n > 0)
        {
			if(strcmp(recvbuf,"endofinput")==0){
				break;
			}
            fwrite(recvbuf, sizeof(char), n, fp);
        }
        /*write file*/
    }
    /*偷說偷說loass rate*/
    close(sock);

    return;
}

void udp_client(char *ip, int port, char *file_name)
{
    int sock;
    if ((sock = socket(PF_INET, SOCK_DGRAM, 0)) < 0)
        error("socket");

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(ip);

    int ret;
    char sendbuf[1024] = {0};
    FILE *fp;
    fp = fopen(file_name, "r");
	struct stat st;
	stat(file_name, &st);
	double size = st.st_size;
	int count = 0;
	int count_per = size/1024/4;

	clock_t begin = clock();
	clock_t beginn = begin;
	clock_t end;
	double time_spent;

    while (!feof(fp))
    {
		count++;
        fread(sendbuf, sizeof(char), sizeof(sendbuf), fp);

        sendto(sock, sendbuf, strlen(sendbuf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

        memset(sendbuf, 0, sizeof(sendbuf));
    
		if(count == 1){
			end = clock();
			time_spent = (double)(end-begin)/CLOCKS_PER_SEC;
			printf("0%% ");
			day();
		}else if(count == count_per){
			begin = end;
			end = clock();
			time_spent = (double)(end-begin)/CLOCKS_PER_SEC;
			printf("25%% ");
			day();
		}else if(count == count_per*2){
			begin = end;
			end = clock();
			time_spent = (double)(end-begin)/CLOCKS_PER_SEC;
			printf("50%% ");
			day();
		}else if(count == count_per*3){
			begin = end;
			end = clock();
			time_spent = (double)(end-begin)/CLOCKS_PER_SEC;
			printf("75%% ");
			day();
		}
	}
    /*偷聽偷聽*/
	memset(sendbuf,0,sizeof(sendbuf));
    sendbuf[0] = 'e';
    sendbuf[1] = 'n';
    sendbuf[2] = 'd';
    sendbuf[3] = 'o';
    sendbuf[4] = 'f';
    sendbuf[5] = 'i';
    sendbuf[6] = 'n';
    sendbuf[7] = 'p';
    sendbuf[8] = 'u';
    sendbuf[9] = 't';
    
	sendto(sock, sendbuf, strlen(sendbuf), 0, (struct sockaddr *)&servaddr, sizeof(servaddr));

	begin = end;
	end = clock();
	time_spent = (double)(end-begin)/CLOCKS_PER_SEC;
	printf("100%% ");
	day();
	time_spent = (double)(end-beginn)/CLOCKS_PER_SEC;
	printf("Total trans time: %f ms\n",time_spent*1000);
	printf("file size : %f MB\n",size/1000000);
	close(sock);
	fclose(fp);
    
	
	
	return;
}


void day()
{
    struct tm *p;
    time_t timep;
    time(&timep);
    p = localtime(&timep);
    printf("%d/%d/%d ", (1900 + p->tm_year), (1 + p->tm_mon), p->tm_mday);
    printf("%d:%.2d:%.2d\n", p->tm_hour, p->tm_min, p->tm_sec);
}
