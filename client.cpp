#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <pthread.h>
#include <string.h>
#include <openssl/sha.h>
#include <sys/stat.h>
//#define PORT 10100
using namespace std;
   
long chunksize=524288;
int PORT1;
string IP1;
struct file_upload_data	{
	char *path;
	int socketfd1;
	//int group_id;
};

struct file_download_data	{
	//int group_id;
	char *filename;
	char *path;
	int socketfd1;
};

long calculatesize(string filename)
{
	int size;
        struct stat buffer;
        size=stat(filename.c_str(),&buffer);
	if(size==0)
		return buffer.st_size;
	else
		return -1;
}

void *server(void *args)	{
	//while(1)	{
	cout<<"Client's server running\n";
	int socketfd,new_socketfd,portno;
	struct sockaddr_in server_addr,client_addr;
	socketfd=socket(AF_INET,SOCK_STREAM,0);
	if(socketfd<0)	{
		cout<<"Error";
	}
	char ipaddress[100000];
	strcpy(ipaddress,IP1.c_str());
	cout<<"client's server's ipaddress: "<<ipaddress<<endl;
	cout<<"client's server's port: "<<PORT1<<endl;
	server_addr.sin_family=AF_INET;
     	server_addr.sin_addr.s_addr=inet_addr(ipaddress);
     	server_addr.sin_port=htons(PORT1);
	int addrlen = sizeof(server_addr);
	if(bind(socketfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) 
              cout<<"Error on binding";
	//cout<<"Server running\n";
     	if(listen(socketfd,5)<0)	
		cout<<"Error listening";
	//cout<<"Listening.. \n";
	while(1)	{
		new_socketfd=accept(socketfd,(struct sockaddr *)&server_addr,(socklen_t*)&addrlen);
		if (new_socketfd < 0) 
			cout<<"ERROR on accept";
		cout<<"Accepted\n";

		char filename1[100000];
		recv(new_socketfd,&filename1,sizeof(filename1),0);
		//string filename(filename1);
		cout<<"Filename received: "<<filename1<<endl;
		FILE *fp=fopen(filename1,"r");
		fseek(fp,0,SEEK_END);
		int size=ftell(fp);
		rewind(fp);
		int n;
		string thisfilename(filename1);
		/*long filesize=calculatesize(thisfilename);
		noofchunks=filesize/chunksize;
		for(int loop=1;loop<=noofchunks;loop++)	{
			
		*/	
		//cout<<"THIS FILE'S SIZE: "<<thisfilesize<<endl;
		send(new_socketfd,&size,sizeof(size),0);
		cout<<"Sent size: "<<size<<endl;
		char buffer[512]={0};
		while((n=fread(buffer,sizeof(char),512,fp))>0 && size>0)	{
			//cout<<"Buffer read: "<<buffer<<endl;
			send(new_socketfd,buffer,n,0);
			memset(buffer,'\0',512);
			size-=n;
		}

		fclose(fp);
		close(new_socketfd);
	}
	//close(new_socketfd);
	close(socketfd);
	//pthread_exit(NULL);
}

void *upload(void *args)	{
	char *path2;
	struct file_upload_data *fdata1;	
	fdata1=(struct file_upload_data *) args;
	string path1="";
	path2=fdata1->path;
	cout<<"Path: "<<fdata1->path;
	int socketfd=fdata1->socketfd1;
	FILE *fp=fopen(path2,"rb");
	fseek(fp,0,SEEK_END);
	int size=ftell(fp);
	rewind(fp);
	int n;
	//send(socketfd,&size,sizeof(size),0);
	/*string fileinfo1="";
	//fileinfo1=
	int point =0;
	for(int k=0;k<strlen(path2);k++)	{
		if(path2[k]=='/')
			point=k;
	}
	char fileinfo[1000000];
	string filename(path2);
	filename=filename.substr(point+1,strlen(path2)-1);*/
	char fileinfo[1000000];
	string filename(path2);
	filename=filename+" "+IP1+" "+to_string(PORT1)+" ";
	cout<<"\nFilename: "<<filename<<endl;
	strcpy(fileinfo,filename.c_str());
	unsigned char buffer[512]={0};
	while((n=fread(buffer,sizeof(char),512,fp))>0 && size>0)	{
		//unsigned char ibuf[] = "compute sha1";
		cout<<"Buffer: "<<buffer<<endl;
		unsigned char obuf[20];
		SHA1(buffer, sizeof(buffer), obuf);
		char buff[10000];
		int i;
		string s="";
	    	for (i = 0; i < 20; i++) {
			int retval=sprintf(buff,"%02x", obuf[i]);
			string str(buff);
			s+=str;
	    	}
		//cout<<s<<endl;
		s=s.substr(0,5);
		char hash[1000000];
		strcpy(hash,s.c_str());
		cout<<"hash: "<<hash<<endl;
		cout<<"fileinfo: "<<fileinfo<<endl;
		strcat(fileinfo,hash);
		cout<<"fileinfo: "<<fileinfo<<endl;
		//send(socketfd,fileinfo,strlen(fileinfo),0);
		memset(buffer,'\0',512);
		//size-=n;
	}
	int size1;
	char fileinfo2[1000000];
	string sss(fileinfo);
	sss+="\n";
	strcpy(fileinfo2,sss.c_str());
	//send(socketfd,&size1,sizeof(fileinfo),0);
	cout<<"Sending fileinfo: "<<fileinfo2<<endl;
	cout<<"length: "<<strlen(fileinfo2)<<endl;
	send(socketfd,fileinfo2,strlen(fileinfo2),0);
	fclose(fp);
	close(socketfd);
	pthread_exit(NULL);
}

void *download(void *args)	{
	cout<<"In download"<<endl;
	char *path2,*fname;
	struct file_download_data *fdata1;	
	fdata1=(struct file_download_data *) args;
	string path1="";
	path2=fdata1->path;
	fname=fdata1->filename;
	int socketfd=fdata1->socketfd1;
	cout<<"Sending "<<fname<<endl;
	send(socketfd,fname,strlen(fname),0);
	char line[1000000];
	recv(socketfd,line,1000000,0);
	cout<<"line: "<<line<<endl;
	

	
	string str(line);
	stringstream ss(str);
	vector<string> token;
	//cout<<"hello2"<<endl;
	while(getline(ss,str,' '))	{
		token.push_back(str);
	}
	string peerip1=token[1];
	char peerip[100000];
	strcpy(peerip,peerip1.c_str());
	char port2[100000];
	strcpy(port2,token[2].c_str());
	int peerport=atoi(port2);
	cout<<"peerip: "<<peerip<<endl;
	cout<<"peerport: "<<peerport<<endl;
	char filename[1000000];
	strcpy(filename,token[0].c_str());
	int clientsocketfd; 
    	struct sockaddr_in peer_addr; 
    	if((clientsocketfd=socket(AF_INET,SOCK_STREAM,0))<0) 	{
        	cout<<"\n Socket creation error \n"; 
        	//return; 
    	}	 

    	peer_addr.sin_family=AF_INET; 
    	peer_addr.sin_port=htons(peerport); 
    	peer_addr.sin_addr.s_addr=inet_addr(peerip);  
    	if(connect(clientsocketfd,(struct sockaddr *)&peer_addr,sizeof(peer_addr))<0) 	{
        	printf("\nConnection Failed \n"); 
        	//return; 
    	} 
	cout<<"Sending filename: "<<filename<<endl;
	send(clientsocketfd,filename,strlen(filename),0);
	//string filepath(path2);
	FILE *fp1=fopen(path2,"w");
	cout<<"path2: "<<path2<<endl;
	char buffer1[512]={0};
	int file_size,n;
	recv(clientsocketfd,&file_size,sizeof(file_size),0);
	cout<<"Received size: "<<file_size<<endl;
	while((n=recv(clientsocketfd,buffer1,512,0))>0 && file_size>0)	{
		//cout<<"buffer got: "<<buffer1<<endl;
		fwrite(buffer1,sizeof(char),n,fp1);
		fflush(fp1);
		memset(buffer1,'\0',512);
		file_size-=n;
	}
	fclose(fp1);
	//close(socketfd);
	close(clientsocketfd);
	pthread_exit(NULL);
}



int main(int argc,char *argv[])	{
	cout<<"I am here";
	//int socketfd; 
	//PORT1=atoi(argv[1]);
	//IP1(argv[2]);
	char command[100000];
	string commandline=argv[1];
	strcpy(command,commandline.c_str());
	char *token=strtok(command,":");
	//cout<<"token[0]: "<<token<<endl;
	int p=1;
	while(token!=NULL)	{
		if(p==1)
			IP1=token;
		else
			PORT1=atoi(token);
		token=strtok(NULL,":");
		p++;
	}
	cout<<"IP1: "<<IP1<<endl;
	cout<<"Port1: "<<PORT1<<endl;
		
	//char filepath[100000];
	string filepath1=argv[2];
	//strcpy(filepath,filepath1.c_str());
	//FILE *trp=fopen(filepath,"r");*/
	ifstream file(filepath1);
	string line;
	p=1;
        //while (getline(file, line))	{
        //    records++;
	getline(file,line);
        file.close();
	string trackerip="",word="";
	int trackerport,c=0;
	char chh[10000];
	for(p=0;p<line.length();p++)	{
		if(line[p]==' ')	{
			if(c==1)	{
				strcpy(chh,word.c_str());
				trackerip=word;
			}
			c++;
			word="";
		}
		else
			word+=line[p];
	}
	trackerport=stoi(word);
	cout<<"trackerip: "<<trackerip<<endl;
	cout<<"trackerport: "<<trackerport<<endl;

    	/*struct sockaddr_in serv_addr; 
    	if((socketfd=socket(AF_INET,SOCK_STREAM,0))<0) 	{
        	cout<<"\n Socket creation error \n"; 
        	//return; 
    	}	 

    	serv_addr.sin_family=AF_INET; 
    	serv_addr.sin_port=htons(trackerport); 
    	serv_addr.sin_addr.s_addr=inet_addr(chh);
	//serv_addr.sin_port=htons(port1); 
    	//serv_addr.sin_addr.s_addr=inet_addr(ip1);  
    	  
    	if(connect(socketfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0) 	{
        	printf("\nConnection Failed \n");
		//cout<<"Connected to server"<<endl; 
        	//return; 
    	}
 	cout<<"hey";
	cout<<"Connected to tracker.........................."<<endl;
	pthread_t serverthread;
	int ret3=pthread_create(&serverthread,NULL,server,NULL);
	if(ret3)
		cout<<"Thread creation failed\n";
	pthread_detach(serverthread);
	pthread_t tid1,tid2;
	string str,str1;
	int ret1,ret2;
	//int i=0;
	//cout<<"Hey\n";
	string userid,passw,userpass;*/
	while(1)	{
		int socketfd;
		struct sockaddr_in serv_addr; 
	    	if((socketfd=socket(AF_INET,SOCK_STREAM,0))<0) 	{
			cout<<"\n Socket creation error \n"; 
			//return; 
	    	}	 

	    	serv_addr.sin_family=AF_INET; 
	    	serv_addr.sin_port=htons(trackerport); 
	    	serv_addr.sin_addr.s_addr=inet_addr(chh);
		//serv_addr.sin_port=htons(port1); 
	    	//serv_addr.sin_addr.s_addr=inet_addr(ip1);  
	    	  
	    	if(connect(socketfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0) 	{
			printf("\nConnection Failed \n");
			//cout<<"Connected to server"<<endl; 
			//return; 
	    	}
	 	cout<<"hey";
		cout<<"Connected to tracker.........................."<<endl;
		


		pthread_t serverthread;
		int ret3=pthread_create(&serverthread,NULL,server,NULL);
		if(ret3)
			cout<<"Thread creation failed\n";
		pthread_detach(serverthread);
		

		pthread_t tid1,tid2;
		string str,str1;
		int ret1,ret2;
		//int i=0;
		//cout<<"Hey\n";
		string userid,passw,userpass;








		cout<<"Enter command: "<<endl;
		getline(cin,str);
		//cout<<"Hello1"<<endl;
		stringstream ss(str);
		vector<string> token;
		//cout<<"hello2"<<endl;
		while(getline(ss,str1,' '))	{
			token.push_back(str1);
		}
		cout<<"Tokens are: "<<endl;
		/*for(int l=0;l<token.size();l++)
			cout<<"token["<<l<<"]: "<<token[l]<<" ";
		cout<<endl;*/
		//char *token=strtok(str," ");
		if(token[0]=="create_user")	{
			string flag1="crt";
			char flag[4];
			strcpy(flag,flag1.c_str());
			//send(socketfd,3,sizeof(flag));
			cout<<"Sending this: "<<flag<<endl;
			send(socketfd,flag,strlen(flag)+1,0);
			cout<<"crt sent"<<endl;
			userid=token[1];
			passw=token[2];
			userpass=userid+" "+passw;
			/*char uchar[10000],pchar[10000];
			strcpy(uchar,userid.c_str());
			strcpy(pchar,passw.c_str());*/
			char upass[10000];
			strcpy(upass,userpass.c_str());
			send(socketfd,upass,strlen(upass)+1,0);		
		}
		/*if(token[0]=="login")	{
			string flag1="log";
			char flag[4];
			strcpy(flag,flag1.c_str());
			//send(socketfd,3,sizeof(flag));
			cout<<"Sending this: "<<flag<<endl;
			send(socketfd,flag,strlen(flag)+1,0);
			cout<<"log sent"<<endl;
			userid=token[1];
			passw=token[2];
			userpass=userid+" "+passw;
			
			char upass[10000];
			strcpy(upass,userpass.c_str());
			send(socketfd,upass,strlen(upass)+1,0);	
			recv(socketfd,logininfo,sizeof(logininfo),0);
			if(strcmp(logininfo,"n")==0)	{
				cout<<"Invalid User ID or password"<<endl;
				continue;
			}
			else
				cout<<"Login successful"<<endl;	
		}*/
		/*if(token[0]=="create_group")	{
			string flag1="crg";
			char flag[4];
			strcpy(flag,flag1.c_str());
			//send(socketfd,3,sizeof(flag));
			cout<<"Sending this: "<<flag<<endl;
			send(socketfd,flag,strlen(flag)+1,0);
			cout<<"crg sent"<<endl;
			groupid1=token[1];
			groupown=groupid1+" "+userid;
			char groupid[10000];
			strcpy(groupid,groupown.c_str());
			send(socketfd,groupid,strlen(groupid)+1,0);
			cout<<"Group created"<<endl;		
		}*/
		/*if(token[0]=="join_group")	{
			string flag1="jog";
			char flag[4];
			strcpy(flag,flag1.c_str());
			//send(socketfd,3,sizeof(flag));
			cout<<"Sending this: "<<flag<<endl;
			send(socketfd,flag,strlen(flag)+1,0);
			cout<<"jog sent"<<endl;
			groupid1=token[1];
			groupown=groupid1+" "+userid;
			char groupid[10000];
			strcpy(groupid,groupown.c_str());
			send(socketfd,groupid,strlen(groupid)+1,0);
			cout<<"Group joined"<<endl;		
		}*/
		/*if(token[0]=="leave_group")	{
			string flag1="leg";
			char flag[4];
			strcpy(flag,flag1.c_str());
			//send(socketfd,3,sizeof(flag));
			cout<<"Sending this: "<<flag<<endl;
			send(socketfd,flag,strlen(flag)+1,0);
			cout<<"leg sent"<<endl;
			groupid1=token[1];
			groupown=groupid1+" "+userid;
			char groupid[10000];
			strcpy(groupid,groupown.c_str());
			send(socketfd,groupid,strlen(groupid)+1,0);
			cout<<"Group left"<<endl;		
		}*/
		if(token[0]=="upload_file")	{
			string flag1="upl";
			char flag[4];
			strcpy(flag,flag1.c_str());
			cout<<"Sending this: "<<flag<<endl;
			send(socketfd,&flag,strlen(flag)+1,0);
			cout<<"upl sent"<<endl;
			struct file_upload_data fdata;
			cout<<"token[1]: "<<token[1]<<endl;
			char path2[10000];
			strcpy(path2,token[1].c_str());
			fdata.path=path2;
			fdata.socketfd1=socketfd;
			cout<<"Hey i am in\n";
			/*ret1=pthread_create(&tid1,NULL,upload,(void *)&fdata);
			if(ret1)
				cout<<"Thread creation failed\n";
			pthread_join(tid1,NULL);*/





			FILE *fp=fopen(path2,"r");
			fseek(fp,0,SEEK_END);
			int size=ftell(fp);
			rewind(fp);
			int n;
			char fileinfo[1000000];
			string filename(path2);
			filename=filename+" "+IP1+" "+to_string(PORT1)+" ";
			cout<<"\nFilename: "<<filename<<endl;
			strcpy(fileinfo,filename.c_str());
			unsigned char buffer[512]={0};
			int noofchunks=size/512;
			cout<<"size: "<<size<<endl;
			cout<<"noofchunks: "<<noofchunks<<endl;
			//vector<int> chunks[noofchunks];
			int loop=0;
			string chunks="";
			for(loop=0;loop<noofchunks;loop++)	
				chunks+=to_string(0);
			cout<<"chunks before: "<<chunks<<endl;
			char chunks1[noofchunks+1];
			strcpy(chunks1,chunks.c_str());
			loop=0;	
			while((n=fread(buffer,sizeof(char),512,fp))>0 && size>0)	{
				//unsigned char ibuf[] = "compute sha1";
				cout<<"Buffer: "<<buffer<<endl;
				chunks1[loop]='1';
				loop++;
				unsigned char obuf[20];
				SHA1(buffer, sizeof(buffer), obuf);
				char buff[10000];
				int i;
				string s="";
			    	for (i = 0; i < 20; i++) {
					int retval=sprintf(buff,"%02x", obuf[i]);
					string str(buff);
					s+=str;
			    	}
				//cout<<s<<endl;
				s=s.substr(0,5);
				char hash[1000000];
				strcpy(hash,s.c_str());
				cout<<"hash: "<<hash<<endl;
				cout<<"fileinfo: "<<fileinfo<<endl;
				strcat(fileinfo,hash);
				cout<<"fileinfo: "<<fileinfo<<endl;
				//send(socketfd,fileinfo,strlen(fileinfo),0);
				memset(buffer,'\0',512);
				//size-=n;
			}
			cout<<"chunks after: "<<chunks1<<endl;
			int size1;
			string chunks2(chunks1);
			memset(chunks1,'\0',sizeof(chunks1));
			chunks2=" "+chunks2;
			char chunks3[10000];
			strcpy(chunks3,chunks2.c_str());
			strcat(fileinfo,chunks3);
			char fileinfo2[1000000];
			string sss(fileinfo);
			sss+="\n";
			strcpy(fileinfo2,sss.c_str());
			//send(socketfd,&size1,sizeof(fileinfo),0);
			cout<<"Sending fileinfo: "<<fileinfo2<<endl;
			cout<<"length: "<<strlen(fileinfo2)<<endl;
			send(socketfd,&fileinfo2,strlen(fileinfo2),0);
			//send(socketfd,&chunks,chunks.size(),0);
			fclose(fp);
			//close(socketfd);
			//pthread_exit(NULL);

			memset(flag,0,sizeof(flag));
			memset(fileinfo2,0,sizeof(fileinfo2));
		}
		if(token[0]=="download_file")	{
			string flag1="dwn";
			char flag[4];
			strcpy(flag,flag1.c_str());
			//send(socketfd,3,sizeof(flag));
			cout<<"Sending this: "<<flag<<endl;
			send(socketfd,flag,strlen(flag)+1,0);
			cout<<"dwn sent"<<endl;
			//cout<<"hey";
			struct file_download_data fdata;
			//cout<<"token[2]: "<<token[2]<<endl;
			char path2[10000],fname[10000];
			strcpy(fname,token[1].c_str());
			strcpy(path2,token[2].c_str());
			cout<<"fname: "<<fname<<endl;
			cout<<"path2: "<<path2<<endl;
			cout<<"socketfd: "<<socketfd<<endl;
			fdata.filename=fname;			
			fdata.path=path2;
			fdata.socketfd1=socketfd;
			cout<<"Hey i am in before download thread\n";
			
			/*ret2=pthread_create(&tid2,NULL,download,(void *)&fdata);
			//cout<<"Hello";
			if(ret2)
				cout<<"Thread creation failed\n";
			pthread_join(tid2,NULL);*/








			/*cout<<"In download"<<endl;
			char *path2,*fname;
			struct file_download_data *fdata1;	
			fdata1=(struct file_download_data *) args;
			string path1="";
			path2=fdata1->path;
			fname=fdata1->filename;
			int socketfd=fdata1->socketfd1;*/
			cout<<"Sending "<<fname<<endl;
			send(socketfd,fname,strlen(fname),0);
			string thisstring="";
			int cunter1=1;
			char line[100000];
			string str;
			int match,chunksreqd;
			recv(socketfd,&match,sizeof(match),0);
			cout<<"No. of peers that have this file: "<<match<<endl;
			recv(socketfd,&chunksreqd,sizeof(chunksreqd),0);
			cout<<"Chunks got: "<<chunksreqd<<endl;
			//while(recv(socketfd,&line,sizeof(line),0)>0)	{
			int cs=chunksreqd,noofcols=0;
			while(cs!=0)	{
				int a=cs%10;
				noofcols++;
				cs=cs/10;
			}
			cout<<"No. of chunks that are required for this file: "<<noofcols<<endl;
			int chunks2[match][noofcols];
			for(int p=0;p<match;p++)	{
				for(int q=0;q<noofcols;q++)
					chunks2[p][q]=1;
			}
			for(int p=0;p<match;p++)	{
				for(int q=0;q<noofcols;q++)
					cout<<chunks2[p][q]<<" ";
				cout<<endl;
			}
			vector<string> arr;
			for(int y=1;y<=match;y++)	{
			//while(1)	{
				int y1=recv(socketfd,&line,sizeof(line),0);
				if(y1==0)
					break;
				if(cunter1==1)	{
					string str1(line);
					str=str1;
					cout<<"str inside loop: "<<str<<endl;
				}				
				
				cout<<"line: "<<line<<endl;
				string line1(line);
				//thisstring+=line1+"\n";
				//arr[cunter1-1]=line1;
				arr.push_back(line1);
				memset(line,'\0',100000);
				cunter1++;
			}
			//cout<<"thisstring: "<<thisstring<<endl;
			cout<<"str: "<<str<<endl;
			for(int loop=0;loop<match;loop++)	{
				cout<<"arr["<<loop<<"]: "<<arr[loop]<<endl;
			}		
			//string str(line);
			stringstream ss(str);
			vector<string> token;
			//cout<<"hello2"<<endl;
			while(getline(ss,str,' '))	{
				token.push_back(str);
			}
			string peerip1=token[1];
			char peerip[100000];
			strcpy(peerip,peerip1.c_str());
			char port2[100000];
			strcpy(port2,token[2].c_str());
			int peerport=atoi(port2);
			cout<<"peerip: "<<peerip<<endl;
			cout<<"peerport: "<<peerport<<endl;
			char filename[1000000];
			strcpy(filename,token[0].c_str());
			int clientsocketfd; 
		    	struct sockaddr_in peer_addr; 
		    	if((clientsocketfd=socket(AF_INET,SOCK_STREAM,0))<0) 	{
				cout<<"\n Socket creation error \n"; 
				//return; 
		    	}	 

		    	peer_addr.sin_family=AF_INET; 
		    	peer_addr.sin_port=htons(peerport); 
		    	peer_addr.sin_addr.s_addr=inet_addr(peerip);  
		    	if(connect(clientsocketfd,(struct sockaddr *)&peer_addr,sizeof(peer_addr))<0) 	{
				printf("\nConnection Failed \n"); 
				//return; 
		    	} 
			cout<<"Sending filename: "<<filename<<endl;
			send(clientsocketfd,&filename,strlen(filename)+1,0);
			//string filepath(path2);
			FILE *fp1=fopen(path2,"w");
			cout<<"path2: "<<path2<<endl;
			char buffer1[512]={0};
			int file_size,n;
			recv(clientsocketfd,&file_size,sizeof(file_size),0);
			cout<<"Received size: "<<file_size<<endl;
			while((n=recv(clientsocketfd,buffer1,512,0))>0 && file_size>0)	{
				//cout<<"buffer got: "<<buffer1<<endl;
				fwrite(buffer1,sizeof(char),n,fp1);
				fflush(fp1);
				memset(buffer1,'\0',512);
				file_size-=n;
			}
			fclose(fp1);
			//close(socketfd);
			close(clientsocketfd);
			//pthread_exit(NULL);
			memset(flag,0,sizeof(flag));
		}

		if(token[0]=="exit")	
			exit(0);
		//i++;
		close(socketfd);
	}
	cout<<"Hey i am OUT"<<endl;
	//close(socketfd);
	//pthread_join(tid1,NULL);
	//pthread_join(tid2,NULL);
	return 0;
}














