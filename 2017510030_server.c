#include<stdio.h>
#include<string.h>	//strlen
#include<stdlib.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include<pthread.h> //for threading , link with lpthread
#include "../json-c/json.h"
#include <stdbool.h>

//the thread function
void *connection_handler(void *);
char global_client_message[2000];
bool flagsend = false;

int main(int argc , char *argv[])
{
	int socket_desc , client_sock , c , *new_sock;
	struct sockaddr_in server , client;
	
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	puts("Socket created");
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 3205 );
	
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed. Error");
		return 1;
	}
	puts("bind done");
	
	//Listen
	listen(socket_desc , 3);
	
	/*
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	*/
	
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	while( (client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
	{
		puts("Connection accepted");
		
		pthread_t sniffer_thread;
		new_sock = malloc(1);
		*new_sock = client_sock;
		
		if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
		{
			perror("could not create thread");
			return 1;
		}
		
		//Now join the thread , so that we dont terminate before the thread
		//pthread_join( sniffer_thread , NULL);
		puts("Handler assigned");
	}
	
	if (client_sock < 0)
	{
		perror("accept failed");
		return 1;
	}
	
	return 0;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
	//Get the socket descriptor
	struct json_object *object, *object1, *tmp, *tmp1, *msgtmp, *passw, *gflag, *tmp2;
	int sock = *(int*)socket_desc;
	int read_size;
	bool flag = true;
	bool groupflag = false;
	char *message , client_message[2000], username[20], groupname[30], password[20];

	//her threadin içerisinde hangi gruplara dahil olduğu tutulmalı
	//Send some messages to the client

	
	//(read_size = recv(sock , client_message , 2000 , 0)) > 0
	//Receive a message from client
	
	while( (read_size = recv(sock , client_message , 2000 , 0)) > 0 )
	{
		//recv(sock , client_message , 2000 , 0);
		if(flag){
			strcpy(username ,client_message);
			flag = false;
			puts(username);
		}else{
			if(strcmp(client_message,"{ }")!=0){
				bzero(global_client_message,2000); 
				strcpy(global_client_message, client_message);
				flagsend = true;
			}
				object = json_object_new_object();
				object1 = json_object_new_object();
				//object = json_object_new_object();
				object = json_tokener_parse(global_client_message);
				//object1 = json_tokener_parse(client_message);
				//tmp= json_object_object_get(object, "from");
				json_object_object_get_ex(object, "to", &tmp);
				//json_object_object_get_ex(object1, "to", &tmp2);
				json_object_object_get_ex(object, "group", &gflag);
				
				//puts(json_object_to_json_string_ext(gflag, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
		
				if(strcmp(json_object_to_json_string_ext(gflag, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY),"\"g\"")==0){
					strcpy(groupname, json_object_to_json_string_ext(tmp, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
					
				}
			
			//puts(json_object_to_json_string_ext(tmp, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY));
			groupflag=flagsend;
			if(strcmp(json_object_to_json_string_ext(tmp, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY),username)==0 && flagsend){
				
				send(sock , global_client_message , strlen(global_client_message),0);
				flagsend=false;
				
			}else if(strcmp(json_object_to_json_string_ext(tmp, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY),groupname)==0 && groupflag){
				send(sock , global_client_message , strlen(global_client_message),0);
				groupflag = false;
				usleep(1000000);
				flagsend = false;
			}else{
				send(sock , "\0" , 1,0);
			}
		}
	}
	
	if(read_size == 0)
	{
		puts("Client disconnected");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("recv failed");
	}
		
	//Free the socket pointer
	free(socket_desc);
	
	return 0;
}
