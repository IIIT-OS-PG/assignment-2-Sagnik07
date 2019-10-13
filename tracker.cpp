#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <bits/stdc++.h>
#include <arpa/inet.h>
#define PORT 10105
using namespace std;


int main()	{
	int socketfd,new_socketfd,portno;
	struct sockaddr_in server_addr,client_addr;
	socketfd=socket(AF_INET,SOCK_STREAM,0);
	if(socketfd<0)	{
		cout<<"Error";
	}
	server_addr.sin_family=AF_INET;
     	server_addr.sin_addr.s_addr=INADDR_ANY;
     	server_addr.sin_port=htons(PORT);
	int addrlen = sizeof(server_addr);
	if(bind(socketfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) 
              cout<<"Error on binding";
     	if(listen(socketfd,5)<0)	
		cout<<"Error listening";
	new_socketfd=accept(socketfd,(struct sockaddr *)&server_addr,(socklen_t*)&addrlen);
     	if (new_socketfd < 0) 
          cout<<"ERROR on accept";
	//char buffer[512]={0};
	FILE *fp=fopen("/home/sagnik/OS_Assign_2/tracker_info.txt","wb");
	char buffer[512]={0};
	int file_size,n;
	recv(new_socketfd,&file_size,sizeof(file_size),0);
	while((n=recv(new_socketfd,buffer,512,0))>0 && file_size>0)	{
		fwrite(buffer,sizeof(char),n,fp);
		memset(buffer,'\0',512);
		file_size-=n;
	}
	fclose(fp);
	
	close(new_socketfd);
	close(socketfd);
}
