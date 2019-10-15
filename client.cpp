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
using namespace std;
int LOGIN=0;
int chunksize=524288;
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

struct peerthreaddata	{
	string filename;
	char *downpath;
	int chunknumber;
	string peeripaddress;
	int peerport;
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
              	cout<<endl;
		//cout<<"Error on binding";
     	if(listen(socketfd,5)<0)	
		cout<<"Error listening";
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
	
	close(socketfd);
	//pthread_exit(NULL);
}
/*
void *server(void *args)	{
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

		int chunk1=1;
		recv(new_socketfd,&chunk1,sizeof(chunk1),0);
		cout<<"Chunk1 received: "<<chunk1<<endl;

		char filename1[1000000];
		recv(new_socketfd,&filename1,sizeof(filename1),0);
		//string filename(filename1);
		cout<<"Filename received: "<<filename1<<endl;


		/*FILE *fp=fopen(filename1,"r");
		fseek(fp,(chunksize*chunk1),SEEK_SET);
		int n;
		cout<<"file ptr location: "<<(chunksize*chunk1)<<endl;
		char buffer[2048]={0};
		int size=(512*1024);
		int count1=0;
		int filesentsize=0;
		while((n=fread(buffer,sizeof(char),2048,fp))>0 && size>0)	{
			cout<<"Buffer read: "<<buffer<<endl;
			//send(new_socketfd,buffer,n,0);
			//fflush(fp);
			memset(buffer,'\0',2048);
			size-=n;
			count1++;
			filesentsize+=n;
		}
		//fclose(fp);
		rewind(fp);
		cout<<"Count1: "<<count1<<endl;
		cout<<"Sending filesentsize: "<<filesentsize<<endl;
		send(new_socketfd,&filesentsize,sizeof(filesentsize),0);
		//FILE *fp1=fopen(filename1,"r");
		fseek(fp,(chunksize*chunk1),SEEK_SET);
		int n1;
		cout<<"file ptr location: "<<(chunksize*chunk1)<<endl;
		char buffer5[2048]={0};
		size=(512*1024);
		count1=0;
		//filesentsize=0;
		/*while((n1=fread(buffer5,sizeof(char),2048,fp))>0 && filesentsize>0)	{
			//cout<<"Buffer read: "<<buffer<<endl;
			send(new_socketfd,buffer5,n1,0);
			fflush(fp);
			memset(buffer5,'\0',2048);
			size-=n1;
			count1++;
			filesentsize-=n1;
		}
		int loop5=filesentsize/2048;
		if((filesentsize%2048)!=0)
			loop5+=1;
		for(int loop1=1;loop1<=loop5;loop1++)	{
			cout<<"Buffer read: "<<buffer5<<endl;
			n1=fread(buffer5,sizeof(char),2048,fp);

			send(new_socketfd,buffer5,n1,0);
			//fflush(fp);
			memset(buffer5,'\0',2048);
			size-=n1;
			count1++;
			filesentsize-=n1;
		}
		cout<<"Count1: "<<count1<<endl;
		cout<<"filesentsize: "<<filesentsize<<endl;
		fclose(fp);
		close(new_socketfd);
		




		FILE *fp=fopen(filename1,"r");
		fseek(fp,(chunksize*chunk1),SEEK_SET);
		cout<<"fp location: "<<ftell(fp)<<endl;
		int n;
		cout<<"file ptr location: "<<(chunksize*chunk1)<<endl;
		char buffer[2048]={0};
		int size=(512*1024);
		int count1=0;
		int filesentsize=0;
		while((n=fread(buffer,sizeof(char),2048,fp))>0 && size>0)	{
			//cout<<"Buffer read: "<<buffer<<endl;
			send(new_socketfd,buffer,n,0);
			//fflush(fp);
			bzero(buffer,sizeof(buffer));
			size-=n;
			count1++;
			filesentsize+=n;
		}
		cout<<"fp location: "<<ftell(fp)<<endl;
		cout<<"Count1: "<<count1<<endl;
		cout<<"Sending filesentsize: "<<filesentsize<<endl;
		fclose(fp);
	}
	
	close(socketfd);
	//pthread_exit(NULL);
}
*/

void *peerthread(void *args)	{
	cout<<"In thread"<<endl;
	char *path2,*downpath1;
	string path7;
	struct peerthreaddata *ptdata1;	
	ptdata1=(struct peerthreaddata *) args;
	string path1="";
	path7=ptdata1->filename;
	downpath1=ptdata1->downpath;
	int chno,peerport1;
	chno=ptdata1->chunknumber;
	cout<<ptdata1->peeripaddress<<endl;
	string peerip;
	peerip=ptdata1->peeripaddress;
	char peerip1[100000];
	strcpy(peerip1,peerip.c_str());
	peerport1=ptdata1->peerport;
	//cout<<"path2: "<<path2<<endl;
	cout<<"chno: "<<chno<<endl;
	cout<<"peerport1: "<<peerport1<<endl;
	cout<<"peerip1: "<<peerip1<<endl;
	cout<<"downpath1: "<<downpath1<<endl;
	//int socketfd=ptdata1->socketfd1;

	int clientsocketfd; 
    	struct sockaddr_in peer_addr; 
    	if((clientsocketfd=socket(AF_INET,SOCK_STREAM,0))<0) 	{
		cout<<"\n Socket creation error \n"; 
		//return; 
    	}	 
    	peer_addr.sin_family=AF_INET; 
   	peer_addr.sin_port=htons(peerport1); 
    	peer_addr.sin_addr.s_addr=inet_addr(peerip1);  
    	if(connect(clientsocketfd,(struct sockaddr *)&peer_addr,sizeof(peer_addr))<0) 	{
		printf("\nConnection Failed \n"); 
		//return; 
    	} 
	char filename2[1000000];
	strcpy(filename2,path7.c_str());
	cout<<"Sending chunknumber: "<<chno<<endl;
	send(clientsocketfd,&chno,sizeof(chno),0);
	cout<<"Sending filename: "<<filename2<<endl;
	send(clientsocketfd,&filename2,sizeof(filename2),0);
	/*cout<<"Sending chunknumber: "<<chno<<endl;
	send(clientsocketfd,&chno,sizeof(chno),0);*/
	FILE *fp1=fopen(downpath1,"w");
	fseek(fp1,(chunksize*chno),SEEK_SET);
	cout<<"fp location: "<<ftell(fp1)<<endl;
	int n;
	char buffer1[2048]={0};
	cout<<"file ptr location: "<<(chunksize*chno)<<endl;
	int file_size;
	/*recv(clientsocketfd,&file_size,sizeof(file_size),0);
	cout<<"Received size: "<<file_size<<endl;
	int count1=0,filerecsize=0;
	/*while((n=recv(clientsocketfd,buffer1,2048,0))>0 && file_size>0)	{
		//cout<<"buffer got: "<<buffer1<<endl;
		fwrite(buffer1,sizeof(char),n,fp1);
		fflush(fp1);
		memset(buffer1,'\0',2048);
		file_size-=n;
		count1++;
		filerecsize+=n;
	}
	int loop5=file_size/2048;
	if((file_size%2048)!=0)
		loop5+=1;
	for(int loop1=1;loop1<=loop5;loop1++)	{
		
		n=recv(clientsocketfd,buffer1,2048,0);
		cout<<"buffer got: "<<buffer1<<endl;		
		fwrite(buffer1,sizeof(char),n,fp1);
		//fflush(fp1);
		memset(buffer1,'\0',2048);
		file_size-=2048;
		count1++;
		filerecsize+=2048;
	}
	rewind(fp1);
	cout<<"Count1: "<<count1<<endl;
	cout<<"filerecsize: "<<filerecsize<<endl;
	cout<<"total size: "<<(filerecsize+(chunksize*chno))<<endl;
	fclose(fp1);*/
	//close(socketfd);




	int count1=0,filerecsize=0;
	int size=(512*1024);
	while((n=recv(clientsocketfd,buffer1,2048,0))>0 && size>0)	{
		//cout<<"buffer got: "<<buffer1<<endl;
		fwrite(buffer1,sizeof(char),n,fp1);
		//fflush(fp1);
		bzero(buffer1,sizeof(buffer1));
		size-=n;
		count1++;
		filerecsize+=n;
	}
	cout<<"fp location: "<<ftell(fp1)<<endl;
	cout<<"Count1: "<<count1<<endl;
	cout<<"filerecsize: "<<filerecsize<<endl;
	fclose(fp1);
	close(clientsocketfd);
	//pthread_exit(NULL);
	
}

int main(int argc,char *argv[])	{
	char command[100000];
	string commandline=argv[1];
	strcpy(command,commandline.c_str());
	char *token=strtok(command,":");
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
		
	string filepath1=argv[2];
	ifstream file(filepath1);
	string line;
	p=1;
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
	    	  
	    	if(connect(socketfd,(struct sockaddr *)&serv_addr,sizeof(serv_addr))<0) 	{
			printf("\nConnection Failed \n");
	    	}
		cout<<"Connected to tracker.........................."<<endl;
		pthread_t serverthread;
		int ret3=pthread_create(&serverthread,NULL,server,NULL);
		if(ret3)
			cout<<"Thread creation failed\n";
		pthread_detach(serverthread);
		pthread_t tid1,tid2;
		string str,str1;
		int ret1,ret2;
		string userid,passw,userpass;



		cout<<"Enter command: "<<endl;
		getline(cin,str);

		stringstream ss(str);
		vector<string> token;
		while(getline(ss,str1,' '))	{
			token.push_back(str1);
		}

		if(token[0]=="create_user")	{
			string flag1="crt";
			char flag[4];
			strcpy(flag,flag1.c_str());
			cout<<"Sending this: "<<flag<<endl;
			send(socketfd,flag,strlen(flag)+1,0);
			cout<<"crt sent"<<endl;
			userid=token[1];
			passw=token[2];
			userpass=userid+" "+passw;
			char upass[10000];
			strcpy(upass,userpass.c_str());
			send(socketfd,upass,strlen(upass)+1,0);		
		}
		if(token[0]=="login")	{
			if(LOGIN==1)	{
				cout<<"Already logged in"<<endl;
				continue;
			}
			string flag1="log";
			char flag[4];
			strcpy(flag,flag1.c_str());
			cout<<"Sending this: "<<flag<<endl;
			send(socketfd,flag,strlen(flag)+1,0);
			cout<<"log sent"<<endl;
			userid=token[1];
			passw=token[2];
			userpass=userid+" "+passw;
			
			char upass[10000];
			strcpy(upass,userpass.c_str());
			send(socketfd,upass,strlen(upass)+1,0);
			char logininfo[10000];	
			recv(socketfd,logininfo,sizeof(logininfo),0);
			if(strcmp(logininfo,"n")==0)	{
				cout<<"Invalid User ID or password"<<endl;
				continue;
			}
			else	{
				cout<<"Login successful"<<endl;
				LOGIN=1;
			}	
		}
		if(token[0]=="logout")	{
			if(LOGIN==0)	{
				cout<<"Not logged in"<<endl;
				continue;
			}
			cout<<"Logged out"<<endl;
			LOGIN=0;
			/*string flag1="log";
			char flag[4];
			strcpy(flag,flag1.c_str());
			cout<<"Sending this: "<<flag<<endl;
			send(socketfd,flag,strlen(flag)+1,0);
			cout<<"log sent"<<endl;
			userid=token[1];
			passw=token[2];
			userpass=userid+" "+passw;
			
			char upass[10000];
			strcpy(upass,userpass.c_str());
			send(socketfd,upass,strlen(upass)+1,0);
			char logininfo[10000];	
			recv(socketfd,logininfo,sizeof(logininfo),0);
			if(strcmp(logininfo,"n")==0)	{
				cout<<"Invalid User ID or password"<<endl;
				continue;
			}
			else	{
				cout<<"Login successful"<<endl;
				LOGIN=1;
			}*/	
		}
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
			if(LOGIN==0)	{
				cout<<"You must login first!"<<endl;
				continue;
			}
			string flag1="upl";
			char flag[4];
			strcpy(flag,flag1.c_str());
			cout<<"Sending this: "<<flag<<endl;
			send(socketfd,&flag,strlen(flag)+1,0);
			cout<<"upl sent"<<endl;
			struct file_upload_data fdata;
			//cout<<"token[1]: "<<token[1]<<endl;
			char path2[10000];
			strcpy(path2,token[1].c_str());
			fdata.path=path2;
			fdata.socketfd1=socketfd;

			FILE *fp=fopen(path2,"r");
			fseek(fp,0,SEEK_END);
			int size=ftell(fp);
			rewind(fp);
			int n;
			char fileinfo[1000000];
			
			int point=0;
			string path3(path2);
			for(int loop=0;loop<path3.length();loop++)	{
				if(path3[loop]=='/')
					point=loop;
			}
			int l=1;
			for(int loop=path3.length()-1;loop>=0;loop--)	{
				if(path3[loop]=='/')	
					break;
				l++;
			}
			path3=path3.substr(point+1,l);
			
			string filename(path2);
			filename=path3+" "+filename+" "+IP1+" "+to_string(PORT1)+" ";
			cout<<"\nFilename: "<<filename<<endl;
			strcpy(fileinfo,filename.c_str());
			unsigned char buffer[512*1024]={0};
			int noofchunks=size/(512*1024);
			cout<<"size: "<<size<<endl;
			cout<<"noofchunks: "<<noofchunks<<endl;

			int loop=0;
			string chunks="";
			for(loop=0;loop<noofchunks;loop++)	
				chunks+=to_string(0);
			//cout<<"chunks before: "<<chunks<<endl;
			char chunks1[noofchunks+1];
			for(loop=0;loop<=noofchunks;loop++)	
				chunks1[loop]='0';
			loop=0;	
			while((n=fread(buffer,sizeof(char),(512*1024),fp))>0 && size>0)	{
				//cout<<"Buffer: "<<buffer<<endl;
				chunks1[loop]='1';
				loop++;
				unsigned char obuf[10];
				SHA1(buffer, sizeof(buffer), obuf);
				char buff[512*1024];
				int i;
				string s="";
			    	for (i = 0; i < 10; i++) {
					int retval=sprintf(buff,"%02x", obuf[i]);
					string str(buff);
					s+=str;
			    	}
				char hash[1000000];
				strcpy(hash,s.c_str());
				strcat(fileinfo,hash);
				memset(buffer,'\0',512*1024);
			}
			chunks1[loop]='\0';
			//cout<<"chunks after: "<<chunks1<<endl;
			int size1;
			string chunks2(chunks1);
			memset(chunks1,'\0',sizeof(chunks1));
			chunks2=" "+chunks2;
			char chunks3[1000000];
			strcpy(chunks3,chunks2.c_str());
			strcat(fileinfo,chunks3);
			char fileinfo2[1000000];
			string sss(fileinfo);
			sss+="\n";
			strcpy(fileinfo2,sss.c_str());
			//send(socketfd,&size1,sizeof(fileinfo),0);
			//cout<<"Sending fileinfo: "<<fileinfo2<<endl;
			cout<<"length: "<<strlen(fileinfo2)<<endl;
			send(socketfd,&fileinfo2,strlen(fileinfo2),0);
			fclose(fp);

			memset(flag,0,sizeof(flag));
			memset(fileinfo2,0,sizeof(fileinfo2));
		}
		if(token[0]=="download_file")	{
			if(LOGIN==0)	{
				cout<<"You must login first!"<<endl;
				continue;
			}
			string flag1="dwn";
			char flag[4];
			strcpy(flag,flag1.c_str());
			cout<<"Sending this: "<<flag<<endl;
			send(socketfd,flag,strlen(flag)+1,0);
			cout<<"dwn sent"<<endl;
			struct file_download_data fdata;
			char path2[10000],fname[10000];
			strcpy(fname,token[1].c_str());
			strcpy(path2,token[2].c_str());
			cout<<"fname: "<<fname<<endl;
			cout<<"path2: "<<path2<<endl;
			char downpath1[10000];
			strcpy(downpath1,token[2].c_str());
			fdata.filename=fname;			
			fdata.path=path2;
			fdata.socketfd1=socketfd;




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
			
			string str;
			int match,chunksreqd,noofcols=0;
			recv(socketfd,&match,sizeof(match),0);
			cout<<"No. of peers that have this file: "<<match<<endl;
			recv(socketfd,&noofcols,sizeof(noofcols),0);
			cout<<"Chunks got: "<<noofcols<<endl;
			
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
			
			int total=0; int p=-1;
			int peers[noofcols];
			while(total<noofcols)	{
				while(p<match)	{
					p++;
					if(chunks2[p][total]==1)	{
						peers[total]=p;
						total++;
						break;
					}
				}
				if(p>=match)
					p=-1;
			}
			
			for(int i=0;i<noofcols;i++)	{
				cout<<peers[i]<<" ";
			}
			vector<string> arr(match);
			char line[100000];
			for(int y=0;y<match;y++)	{
				
				int y1=recv(socketfd,&line,sizeof(line),0);
				//cout<<"y1: "<<y1<<endl;
				if(y1==0)
					break;
				if(cunter1==1)	{
					string str1(line);
					str=str1;
				}				
				string line1(line);
				arr[y]=line1;
				memset(line,'\0',100000);
				cunter1++;
			}
			vector<string> peeripaddr1(match),peerfilepath1(match),peeripaddr(noofcols),peerfilepath(noofcols),peerportno1(match);
			vector<int> peerportno(noofcols);
			//cout<<"str: "<<str<<endl;
			for(int loop=0;loop<match;loop++)	{
				//cout<<"arr["<<loop<<"]: "<<arr[loop]<<endl;
				stringstream ss1(arr[loop]);
				string str5="";
				vector<string> token4;
				while(getline(ss1,str5,' '))	{
					//cout<<"str5: "<<str5<<endl;
					token4.push_back(str5);
				}
				peeripaddr1[loop]=token4[2];
				peerfilepath1[loop]=token4[1];
				peerportno1[loop]=token4[3];
			}



			stringstream ss(str);
			vector<string> token;
			while(getline(ss,str,' '))	{
				token.push_back(str);
			}
			string peerip1=token[2];
			char peerip[100000];
			strcpy(peerip,peerip1.c_str());
			char port2[100000];
			strcpy(port2,token[3].c_str());
			int peerport=atoi(port2);
			cout<<"peerip: "<<peerip<<endl;
			cout<<"peerport: "<<peerport<<endl;
			char filename[1000000];
			strcpy(filename,token[1].c_str());

			/*for(int loop=0;loop<noofcols;loop++)	{
				peeripaddr[loop]=peeripaddr1[peers[loop]];
				peerfilepath[loop]=peerfilepath1[peers[loop]];
				stringstream ss5(peerportno1[peers[loop]]);
				ss5>>peerportno[loop];
			}
			for(int loop=0;loop<noofcols;loop++)	{
				cout<<" peeripaddr["<<loop<<"]= "<<peeripaddr[loop];
				cout<<" peerfilepath["<<loop<<"]= "<<peerfilepath[loop];
				cout<<" peerportno["<<loop<<"]= "<<peerportno[loop];
				cout<<endl;
			}
          		
			

			
			pthread_t tid[noofcols];
			vector<struct peerthreaddata> ptdata(noofcols);
			for(int loop=0;loop<noofcols;loop++)	{
				//pthread_t peerthread;
				//cout<<"loop: "<<loop<<endl;
				//struct peerthreaddata ptdata;
				//cout<<"token[1]: "<<token[1]<<endl;
				//char path2[10000];
				//strcpy(path2,token[1].c_str());
				char abcd[100000];
				strcpy(abcd,peerfilepath[loop].c_str());
				ptdata[loop].filename=peerfilepath[loop];
				//ptdata[loop].chunknumber=peers[loop];
				ptdata[loop].chunknumber=loop;
				ptdata[loop].peeripaddress=peeripaddr[loop];
				ptdata[loop].peerport=peerportno[loop];
				ptdata[loop].downpath=downpath1;
				cout<<"BEFORE THREAD"<<endl;
				cout<<"filename: "<<ptdata[loop].filename<<endl;
				//cout<<"peers["<<loop<<"]: "<<ptdata[loop].chunknumber<<endl;
				cout<<"peerportno["<<loop<<"]: "<<ptdata[loop].peerport<<endl;
				cout<<"peeripaddr["<<loop<<"]: "<<ptdata[loop].peeripaddress<<endl;
				cout<<"downpath["<<loop<<"]: "<<ptdata[loop].downpath<<endl;
				/*int ret4=pthread_create(&tid[loop],NULL,peerthread,(void *)&ptdata[loop]);
				if(ret4)
					cout<<"Thread creation failed\n";
				//pthread_detach(tid[loop]);	
						
			}*/
			//pthread_join(tid[loop],NULL);
			/*for(int loop=0;loop<noofcols;loop++)	{
				pthread_join(tid[loop],NULL);
			}*/

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
		close(socketfd);
	}
	return 0;
}














