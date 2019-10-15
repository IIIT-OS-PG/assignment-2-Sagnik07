#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <bits/stdc++.h>
#include <arpa/inet.h>
#include <cstdlib>
#include <pthread.h>
using namespace std;

struct group	{
	string groupid;
	string groupowner;
	vector<string> groupmember;
};
//struct group group1[];

unordered_map<string,string> mp;

void *trackerthread(void *args)	{
	int new_socketfd=*((int *)args);
	char flag[4]={0};
	int flag_size;
	int flag_read,n;
	while(n=recv(new_socketfd,&flag,sizeof(flag),0)>0)	{
		//cout<<"RECEIVING................"<<endl;
		int ret1,ret2;
		cout<<"Waiting for receive"<<endl;
		pthread_t tid1,tid2;
		string str,str1;
		
		string flag2(flag);
		flag2=flag2.substr(0,3);
		cout<<"Received flag: "<<flag<<endl;
		if(flag2=="crt")	{
			ofstream fout;
			fout.open("user_info.txt",ios::app);
			char upass2[1000];
			recv(new_socketfd,&upass2,sizeof(upass2),0);
			cout<<upass2<<endl;
			string upass(upass2);
			string upass1,userid,password;
			stringstream ss(upass);
			vector<string> token;
			while(getline(ss,upass1,' '))	{
				token.push_back(upass1);
			}
			userid=token[0];
			password=token[1];
			mp[userid]=password;
			fout<<upass<<endl;
			fout.close();
		}
		else if(flag2=="log")	{
			fstream fin; 
    			string word,key=""; 
    			fin.open("user_info.txt"); 
			int c=0;
		    	while(fin>>word)	{ 
				if(c==0)
					key=word;
				else
					mp[key]=word;
				c=(c+1)%2;
			} 
			char upass2[1000];
			recv(new_socketfd,&upass2,sizeof(upass2),0);
			cout<<upass2<<endl;
			string upass(upass2);
			string upass1,userid,password;
			stringstream ss(upass);
			vector<string> token;
			while(getline(ss,upass1,' '))	{
				token.push_back(upass1);
			}
			userid=token[0];
			password=token[1];
			string login1;
			if(mp.find(userid)!=mp.end() && mp[userid]==password)	
				login1="y";
			else
				login1="n";
			char logininfo[2];
			strcpy(logininfo,login1.c_str());
			send(new_socketfd,logininfo,strlen(logininfo)+1,0);
			fin.close();
		}
		/*else if(flag2=="crg")	{
			char upass2[1000];
			recv(new_socketfd,&upass2,sizeof(upass2),0);
			cout<<upass2<<endl;
			string upass(upass2);
			string upass1,userid,password;
			stringstream ss(upass);
			vector<string> token;
			while(getline(ss,upass1,' '))	{
				token.push_back(upass1);
			}
			//group1;
			group1.groupid=token[0];
			group1.groupowner=token[1];
			group1.groupmember.push_back(token[1]);
		}*/
		/*else if(flag2=="jog")	{
			char upass2[1000];
			recv(new_socketfd,&upass2,sizeof(upass2),0);
			cout<<upass2<<endl;
			string upass(upass2);
			string upass1,userid,password;
			stringstream ss(upass);
			vector<string> token;
			while(getline(ss,upass1,' '))	{
				token.push_back(upass1);
			}
			//group1;
			group1.groupid=token[0];
			group1.groupmember.push_back(token[1]);
		}*/
		/*else if(flag2=="leg")	{
			char upass2[1000];
			recv(new_socketfd,&upass2,sizeof(upass2),0);
			cout<<upass2<<endl;
			string upass(upass2);
			string upass1,userid,password;
			stringstream ss(upass);
			vector<string> token;
			while(getline(ss,upass1,' '))	{
				token.push_back(upass1);
			}
			group1.groupid=token[0];
			group1.groupmember.push_back(token[1]);
			int leaveflag=0;
			for(auto it=group1.groupmember.begin();it!=group1.groupmember.end();it++)	{
				if(*h==token[1])	{
					leaveflag=1;
					group1.groupmember.erase(it);
				}
			}
					
		}*/
		if(flag2=="upl")	{
			FILE *fp=fopen("client_info.txt","a+");
			char buffer[100000]={0};
			int file_size=1,n;
			while((n=recv(new_socketfd,buffer,sizeof(100000),0))>0)	{ 
				fwrite(buffer,sizeof(char),n,fp);
				fflush(fp);
				memset(buffer,'\0',100000);
			}
			fclose(fp);
			memset(flag,0,sizeof(flag));
		}		
		else if(strcmp(flag,"dwn")==0)	{
			char fileneeded[10000];
			recv(new_socketfd,fileneeded,10000,0);
			cout<<"fileneeded: "<<fileneeded<<endl;
			string clientpath1="client_info.txt";
			char clientpath[1000000];
			strcpy(clientpath,clientpath1.c_str());
			ifstream file(clientpath);
			string line="";
			int p=1;
			char ch[100000];
			char ch1[1000000];
			char ch5[100000];
			int match=0,chunksreqd=0;
			vector<string> tobesent(match);
			while (getline(file, line))	{
				strcpy(ch,line.c_str());
				strcpy(ch5,line.c_str());
				char *token=strtok(ch," ");
				if(strcmp(token,fileneeded)==0)	{
					match++;
					//cout<<"ch5: "<<ch5<<endl;
					char *token3=strtok(ch5," ");
					int loopc=0;
					while(token3!=NULL)	{
						//cout<<"token3: "<<token3<<" loopc: "<<loopc<<endl;
						if(loopc==5)	{
							string str11(token3);
							chunksreqd=str11.length();
							//cout<<"Chunksreqd: "<<chunksreqd<<endl;
						}
						token3=strtok(NULL," ");
						loopc++;
					}
				}
			}
			file.close();
			ifstream file1(clientpath);
			line="";
			cout<<"Sending match: "<<match<<endl;
			send(new_socketfd,&match,sizeof(match),0);
			cout<<"Sending chunksreqd: "<<chunksreqd<<endl;
			send(new_socketfd,&chunksreqd,sizeof(chunksreqd),0);
			while (getline(file1, line))	{
				strcpy(ch,line.c_str());
				char *token=strtok(ch," ");
				if(strcmp(token,fileneeded)==0)	{
					strcpy(ch1,line.c_str());
					send(new_socketfd,&ch1,strlen(ch1)+1,0);
					memset(ch1,'\0',sizeof(ch1));
				}
				memset(ch1,'\0',sizeof(ch1));
				memset(ch,'\0',sizeof(ch1));
			}

			file1.close();

		}
		memset(flag,0,sizeof(flag));
	}
	if(n < 0) {
        	printf("recv error: %s\n", strerror(errno));
    	}
	//cout<<"Out of client 1's loop"<<endl;
	close(new_socketfd);
}

int main(int argc,char *argv[])	{
	string filepath1=argv[1];
	//int trackerno=atoi(argv[2]);
	ifstream file(filepath1);
	string line;
	int p=1;
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
	int socketfd,portno; 
	struct sockaddr_in server_addr,client_addr;
	socketfd=socket(AF_INET,SOCK_STREAM,0);
	if(socketfd<0)	{
		cout<<"Error";
	}
	server_addr.sin_family=AF_INET;
	server_addr.sin_addr.s_addr=inet_addr(chh);     	
	server_addr.sin_port=htons(trackerport);
	int addrlen = sizeof(server_addr);
	if(bind(socketfd,(struct sockaddr *)&server_addr,sizeof(server_addr)) < 0) 
        	cout<<"Error on binding";
     	if(listen(socketfd,5)<0)	
		cout<<"Error listening";
	cout<<"Tracker listening..."<<endl;
	pthread_t tid1,tid2;
	string str,str1;
	int ret1,ret2;
	int i=0;
	while(1)	{
		int new_socketfd=accept(socketfd,(struct sockaddr *)&server_addr,(socklen_t*)&addrlen);
	     	if (new_socketfd < 0) 
		  cout<<"ERROR on accept";
		cout<<"Accepted"<<endl;
		char flag[100000];
		int flag_size;
		int flag_read;
		pthread_t tid;
		int ret=pthread_create(&tid,NULL,trackerthread,(void *)&new_socketfd);
		if(ret)
			cout<<"Tracker thread creation failed"<<endl;
		pthread_detach(tid);

	}

	
	close(socketfd);
}
