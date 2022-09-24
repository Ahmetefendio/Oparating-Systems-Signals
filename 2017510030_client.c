#include <stdio.h>    //printf
#include <string.h>    //strlen
#include <stdlib.h>
#include <sys/socket.h>    //socket
#include <arpa/inet.h>    //inet_addr
#include <unistd.h>
#include "../json-c/json.h"
#include <stdbool.h>
#include <pthread.h>
/*    ******USAGE******
gcc client.c -o client -ljson-c -lpthread

gcreate: -greate phone_number group_name (-gcreate 123456 group1)
join: -join phone_number/group_name groupflag (-join 123456)(-join group1 g) give "g" to join a group
exit: -exit or -exit group_name
whoami: -whoami
send: -send message

*/
//the thread function
void *sendrev_handler(void *);
char control[6];
int main(int argc , char *argv[])
{

    	//First part is same as the lab code.
    	int sock, *sock1;
	
    	struct sockaddr_in server;
    	struct json_object *object, *tmp, *tmp1, *msgtmp, *passw, *cmd, *empty, *gflag;
    	char message[300] , command[500], pass[100], msgtemp[300];

    	//Create socket
    	sock = socket(AF_INET , SOCK_STREAM , 0);
	sock1 = malloc(1);
    	*sock1 = sock;
    	if (sock == -1)
    	{
        	printf("Could not create socket");
    	}
    	puts("Socket created");

    	server.sin_addr.s_addr = inet_addr("127.0.0.1");
    	server.sin_family = AF_INET;
    	server.sin_port = htons( 3205 );

    	//Connect to remote server
    	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
    	{
        	perror("connect failed. Error");
        	return 1;
    	}

    	puts("Connected\n");
    	char username[100];
    	printf("Enter username : ");

	fgets(username,255,stdin);

	cmd = json_object_new_string(username);
	if( send(sock ,json_object_to_json_string_ext(cmd, JSON_C_TO_STRING_SPACED | JSON_C_TO_STRING_PRETTY), 20, 0) < 0)
        {
            	puts("Send failed");
            	return 1;
        }
	
    	//keep communicating with server
    	while(1)
   	{
		puts("command:");
		object = json_object_new_object();
        	bzero(command,256);
		fgets(command,255,stdin);
		char *ptr = strtok(command, " ");
		char *token[3];
		int i = 0;
		while (ptr != NULL)
    		{
        		token[i++] = ptr;
        		ptr = strtok (NULL, " ");
    		}
	
		if(strcmp(token[0],"-gcreate")==0){
			pthread_t c_thread;
		
			if( pthread_create( &c_thread , NULL ,  sendrev_handler , (void*) sock1) < 0)
			{
				perror("could not create thread");
				return 1;
			}
			bool flag = true;
			bzero(control,6);
			puts("Enter password : ");
			bzero(pass,100);
			fgets(pass,100,stdin);
			//printf("%s",pass);
			int len;
			len = strlen(token[2]);
			if( token[2][len-1] == '\n' ){token[2][len-1] = 0;}
			
			// json handling
			tmp = json_object_new_string(token[1]);
			tmp1 = json_object_new_string(token[2]);
			json_object_object_add(object, "from", tmp);
			json_object_object_add(object, "to", tmp1);
			passw = json_object_new_string(pass);
			json_object_object_add(object, "password", passw);
			gflag = json_object_new_string("g");
			json_object_object_add(object, "group", gflag);

			while(flag){
				empty = json_object_new_object();
				puts("enter: ");
				bzero(message,256);
				fgets(message,255,stdin);
				bzero(msgtemp,300);
				strcpy(msgtemp ,message);
				memcpy( control, &message, 5 );
				control[5] = '\0';
				char *ptr1 = strtok(msgtemp, " ");
				char *token1[3];
				int i = 0;
				while (ptr1 != NULL)
    				{
        				token1[i++] = ptr1;
        				ptr1 = strtok (NULL, " ");
    				}
				len = strlen(token1[1]);
				if( token1[1][len-1] == '\n' ){
    				token1[1][len-1] = 0;}
				if(i<2){token1[1] = " ";}
				if(strcmp(control,"-send")==0){
					memcpy( message, &message[6],strlen(message)-5);
					msgtmp = json_object_new_string(message);
					json_object_object_add(object, "message", msgtmp);
					if( send(sock ,json_object_to_json_string(object), 157, 0) < 0)
        				{
            					puts("Send failed");
            					return 1;
        				}
				
			
				}
				else if( send(sock ,json_object_to_json_string(empty), 157, 0) < 0)
        			{
            				puts("Send failed");
            				return 1;
        			}
				if(strcmp(control,"-exit")==0){
					if(strcmp(token[2],token1[1])==0){
						flag = false;}
					else{exit(0);}
				}
		
			}
		
		}
	
		if(strcmp(token[0],"-join")==0){
			pthread_t j_thread;
		
			if( pthread_create( &j_thread , NULL ,  sendrev_handler , (void*) sock1) < 0)
			{
				perror("could not create thread");
				return 1;
			}
			bool flag = true;
			bzero(control,6);
			puts("Enter password : ");
			bzero(pass,100);
			fgets(pass,100,stdin);

			int len;
			len = strlen(token[2]);
			if( token[2][len-1] == '\n' ){token[2][len-1] = 0;}
			tmp1 = json_object_new_string(token[1]);
			gflag = json_object_new_string(token[2]);
			tmp = json_object_new_string(username);
			json_object_object_add(object, "from", tmp);
			json_object_object_add(object, "to", tmp1);
			json_object_object_add(object, "group", gflag);
			passw = json_object_new_string(pass);
			json_object_object_add(object, "password", passw);
		
			while(flag){
				empty = json_object_new_object();
				puts("enter: ");
				bzero(message,256);
				fgets(message,255,stdin);
				bzero(msgtemp,300);
				strcpy(msgtemp ,message);
				memcpy( control, &message, 5 );
				control[5] = '\0';
				char *ptr1 = strtok(msgtemp, " ");
				char *token1[3];
				int i = 0;
				while (ptr1 != NULL)
    				{
        				token1[i++] = ptr1;
        				ptr1 = strtok (NULL, " ");
    				}
				len = strlen(token1[1]);
				if( token1[1][len-1] == '\n' ){
				printf("girdik");
    				token1[1][len-1] = 0;}
				if(i<2){token1[1] = " ";}
				if(strcmp(control,"-send")==0){
					memcpy( message, &message[6],strlen(message)-5);
					msgtmp = json_object_new_string(message);
					json_object_object_add(object, "message", msgtmp);
					if( send(sock ,json_object_to_json_string(object), 157, 0) < 0)
        				{
            					puts("Send failed");
            					return 1;
        				}
				
				}
				else if( send(sock ,json_object_to_json_string(empty), 157, 0) < 0)
        			{
            				puts("Send failed");
            				return 1;
        			}
				if(strcmp(control,"-exit")==0){
					if(strcmp(token[1],token1[1])==0){
						flag = false;}
					else{exit(0);}
				}
		
			}
		
		}
		int len;
		len = strlen(token[0]);
			if( token[0][len-1] == '\n' ){
    			token[0][len-1] = 0;}
		if(strcmp(token[0],"-whoami")==0){
			puts(username);	
		}
		if(strcmp(token[0],"-exit")==0){
			exit(0);	
		}

    	}

    	close(sock);
   	return 0;
}
void *sendrev_handler(void *socket_desc){
	int sock = *(int*)socket_desc;
	char server_reply[2000];
	bool flag = true;
	struct json_object *empty1;
	empty1 = json_object_new_object();
		
	while(flag){
		if( send(sock ,json_object_to_json_string(empty1), 157, 0) < 0)
        		{
            			puts("Send failed");
            			break;
        		}
		if( recv(sock , server_reply , 2000 , 0) < 0)
        		{
           			 puts("recv failed");
            			break;
        		}
		
		if(strcmp(server_reply,"\0") != 0){
			puts(server_reply);
		}
		usleep(500000);
		if(strcmp(control,"-exit")==0){
			flag = false;
		}
		
	}
	return 0;
}
