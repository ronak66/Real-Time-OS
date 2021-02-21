// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <signal.h>
#include <pthread.h>
#include "../include/utils.h"
#define PORT 5000 


int sock = 0;
char client_name[20],client_number[10];

void *send_message(){
	while(1){
		struct Message packet;
		char msg[1024];
		char reciever_number[10];
		int msg_type;

		printf("Enter your message: ");
		scanf("%[^\n]%*c", msg); 
		printf("Enter message type 1 or 2 (1: To all, 2: Particular client): ");
		scanf("%d%*c",&msg_type);
		if(msg_type == 2){
			printf("Enter reciever number: ");
			scanf("%[^\n]%*c", reciever_number);
			// fflush(stdout);
			// fgets(reciever_number, sizeof(reciever_number), stdin);
		}
		strcpy(packet.sender, client_number);
		strcpy(packet.sender_name, client_name);
		packet.message_type = msg_type;
		strcpy(packet.message, msg);
		strcpy(packet.receiver, reciever_number);
		send(sock , &packet , sizeof(packet) , 0); 
		printf("-----Message Sent-----\n"); 
	}
}

void *recieve_message(){
	while(1){
		struct Message msg;
		int valread = recv(sock, &msg, sizeof(msg), 0);
		// printf("Recieved Message from %s: %s\n",msg.sender,msg.message); 
		fprintf(stdout,"\33[2K\r== %s (Ph.No. %s): %s\nEnter your message: ", msg.sender_name,msg.sender,msg.message);
        fflush(stdout);
	}
}

struct Data get_data(){
	printf("Input Name: ");
	scanf("%[^\n]%*c", client_name);
	printf("Input Number: ");
	scanf("%[^\n]%*c", client_number);
	struct Data data ;
	strcpy(data.name, client_name);
	strcpy(data.number, client_number);
	// data.number = number;
	printf("%s and %s\n", client_name, client_number);
	return data;
}

int main(int argc, char const *argv[]) 
{ 
	int valread; 
	struct sockaddr_in serv_addr; 
	char *hello = "Hello from client"; 
	char buffer[1024] = {0};

	pthread_t send_thread, recv_thread;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	
	// Convert IPv4 and IPv6 addresses from text to binary form 
	//if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 	
	if(inet_pton(AF_INET, "13.82.142.97", &serv_addr.sin_addr)<=0) 
	
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	}


	struct Data data;
	data = get_data();
	send(sock, &data, sizeof(data),0);

	pthread_create(&send_thread, NULL, send_message, NULL);
	pthread_create(&recv_thread, NULL, recieve_message, NULL);


	pthread_join(send_thread, NULL);
	pthread_join(recv_thread, NULL);
	return 0; 
} 
