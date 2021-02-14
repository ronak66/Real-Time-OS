// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <signal.h>
#include <pthread.h>
#include "../message.h"

#define PORT 5000 
#define MAX_CLIENTS 3
int sock = 0;
int number_of_clients = 0;
pthread_mutex_t memberRegistrationMutex;

int client_sock[MAX_CLIENTS] = {0};
char client_number[MAX_CLIENTS][10];
char client_name[MAX_CLIENTS][20];
pthread_t client_thread[MAX_CLIENTS];

void *send_message(){
	while(1){
		char msg[1024];
		printf("Enter your message: \n");
		scanf("%[^\n]%*c", msg); 
		send(sock , msg , strlen(msg) , 0); 
		printf("-----Message Sent-----\n"); 
	}
}

void *recieve_message(){
	while(1){
		struct Message msg;
		int valread = recv(sock, &msg, sizeof(msg), 0);
		printf("Recieved Message from Client: %s and %d\n",msg.message,msg.message_type); 
	}
}

void *client_handler(void *socket_fd){
	int client_sock_fd = *(int *)socket_fd;
	struct Register data;
	int valread = recv(sock, &data, sizeof(data), 0);
	printf("Registering: %s and %s\n",data.name,data.number);
	pthread_mutex_lock(&memberRegistrationMutex);
	client_sock[number_of_clients] = client_sock_fd;
	strcpy(client_number[number_of_clients], data.number);
	strcpy(client_name[number_of_clients], data.name);
	number_of_clients++;
	pthread_mutex_unlock(&memberRegistrationMutex);

	printf("Numer of Clients: %d\n",number_of_clients);
	struct Message msg;
	while(recv(client_sock_fd, &msg, sizeof(msg), 0)) {
	// recv(connection_fd, recvBuffer, MAXLENGTH, 0);
		printf("Recieved Message from %s of type %d: %s\n",msg.sender,msg.message_type,msg.message);
		if(msg.message_type == 2){
			for(int j=0;j<number_of_clients;j++){
				if(strcmp(client_number[j], msg.receiver) == 0){
					send(client_sock[j], &msg, sizeof(msg),0); 
				}
			}
		}
		else{
			for(int j=0;j<number_of_clients;j++){
				if(strcmp(client_number[j], msg.sender) != 0){
					send(client_sock[j], &msg, sizeof(msg),0); 
				}
			}
		}
	}
	
}

int main(int argc, char const *argv[]) 
{ 
	int server_fd, valread; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 


	pthread_t thread1, thread2;
	// Creating socket file descriptor 
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
	{ 
		perror("socket failed"); 
		exit(EXIT_FAILURE); 
	} 
	
	// Forcefully attaching socket to the port 8080 
	if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
												&opt, sizeof(opt))) 
	{ 
		perror("setsockopt"); 
		exit(EXIT_FAILURE); 
	} 
	address.sin_family = AF_INET; 
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
	
	// Forcefully attaching socket to the port 8080 
	if (bind(server_fd, (struct sockaddr *)&address, 
								sizeof(address))<0) 
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
	} 
	if (listen(server_fd, 3) < 0) 
	{ 
		perror("listen"); 
		exit(EXIT_FAILURE); 
	} 
	while(1){
		if ((sock = accept(server_fd, (struct sockaddr *)&address, 
					(socklen_t*)&addrlen))<0){ 
			perror("accept"); 
			exit(EXIT_FAILURE); 
		} 
		pthread_create(&client_thread[number_of_clients],NULL,client_handler, (void *)&sock);		
	}

	// pthread_join(thread1, NULL);
	// pthread_join(thread2, NULL);
	return 0; 
} 
