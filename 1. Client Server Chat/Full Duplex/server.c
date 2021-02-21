// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <signal.h>
#include <pthread.h>
#define PORT 5000 

int sock = 0;

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
		char buffer[1024] = {0}; 
		int valread = read( sock , buffer, 1024); 
		printf("Recieved Message from Client: %s\n",buffer ); 
	}
}

int main(int argc, char const *argv[]) 
{ 
	int server_fd, valread; 
	struct sockaddr_in address; 
	int opt = 1; 
	int addrlen = sizeof(address); 
	char *hello = "Hello from server"; 

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
	if ((sock = accept(server_fd, (struct sockaddr *)&address, 
					(socklen_t*)&addrlen))<0) 
	{ 
		perror("accept"); 
		exit(EXIT_FAILURE); 
	} 
	// printf("%d\n",sock);
	pthread_create(&thread1, NULL, recieve_message, NULL);
	pthread_create(&thread2, NULL, send_message, NULL);

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	return 0; 
} 
