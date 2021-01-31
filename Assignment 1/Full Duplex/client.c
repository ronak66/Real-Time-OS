// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
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
	int valread; 
	struct sockaddr_in serv_addr; 
	char *hello = "Hello from client"; 
	char buffer[1024] = {0};

	pthread_t thread1, thread2;

	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	} 

	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	
	// Convert IPv4 and IPv6 addresses from text to binary form 
	if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 	
	// if(inet_pton(AF_INET, "52.188.118.148", &serv_addr.sin_addr)<=0) 
	
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	} 
	// send(sock , hello , strlen(hello) , 0 ); 
	// printf("%d\n",sock);
	pthread_create(&thread1, NULL, send_message, NULL);
	pthread_create(&thread2, NULL, recieve_message, NULL);
	// pthread_join();

	pthread_join(thread1, NULL);
	pthread_join(thread2, NULL);
	return 0; 
} 
