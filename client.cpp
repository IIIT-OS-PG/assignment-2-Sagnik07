#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <bits/stdc++.h>
#include <arpa/inet.h>
#define PORT 10105
using namespace std;
   
int main(int argc,char const *argv[]) 	{
    int socketfd; 
    struct sockaddr_in serv_addr; 
    if((socketfd=socket(AF_INET,SOCK_STREAM,0))<0) 	{
        cout<<"\n Socket creation error \n"; 
        return -1; 
    } 

    serv_addr.sin_family=AF_INET; 
    serv_addr.sin_port=htons(PORT); 
    serv_addr.sin_addr.s_addr=inet_addr("127.0.0.1");  
    if(connect(socketfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0) 	{
        printf("\nConnection Failed \n"); 
        return -1; 
    } 

	FILE *fp=fopen("/home/sagnik/abc.txt","rb");
	fseek(fp,0,SEEK_END);
	int size=ftell(fp);
	rewind(fp);
	int n;
	send(socketfd,&size,sizeof(size),0);

	char buffer[512]={0};
	while((n=fread(buffer,sizeof(char),512,fp))>0 && size>0)	{
		send(socketfd,buffer,n,0);
		memset(buffer,'\0',512);
		size-=n;
	}

	fclose(fp);
	close(socketfd);

    return 0;
}
