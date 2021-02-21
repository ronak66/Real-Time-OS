// Server side C/C++ program to demonstrate Socket programming 
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h> 
#include <signal.h>
#include <pthread.h>
#include "../include/utils.h"

#define PORT 5000 
#define MAX_CLIENTS 10
#define BUFSIZE 1024
int sock = 0;
int number_of_clients = 0;
pthread_mutex_t memberRegistrationMutex;

int client_sock[MAX_CLIENTS] = {0};
char client_number[MAX_CLIENTS][10];
char client_name[MAX_CLIENTS][20];
pthread_t client_thread[MAX_CLIENTS];

static ssize_t loop_read(int fd, void* data, size_t size) {
    ssize_t ret = 0;
    while (size > 0) {
        ssize_t r;

        if ((r = read(fd, data, size)) < 0)
            return r;

        if (r == 0)
            break;

        ret += r;
        data = (struct Message*)data + r;
        size -= (size_t) r;
    }
    return ret;
}

static ssize_t loop_write(int fd, const void* data, size_t size) {
    ssize_t ret = 0;
    while (size > 0) {
        ssize_t r;

        if ((r = write(fd, data, size)) < 0)
            return r;

        if (r == 0)
            break;

        ret += r;
        data = (const struct Message *)data + r;
        size -= (size_t) r;
    }
    return ret;
}

void *client_handler(void *socket_fd){
	int client_sock_fd = *(int *)socket_fd;
	struct Data data;
	int valread = recv(sock, &data, sizeof(data), 0);
	printf("Registering: %s and %s\n",data.name,data.number);
	pthread_mutex_lock(&memberRegistrationMutex);
	client_sock[number_of_clients] = client_sock_fd;
	strcpy(client_number[number_of_clients], data.number);
	strcpy(client_name[number_of_clients], data.name);
	number_of_clients++;
	pthread_mutex_unlock(&memberRegistrationMutex);

	printf("Numer of Clients: %d\n",number_of_clients);
	while(1) {
		// struct Message message;
		uint8_t buf[BUFSIZE];
		// valread = read(client_fd, &message, sizeof(message));
		valread = loop_read(client_sock_fd, buf, sizeof(buf)); 
		for(int i=0;i<number_of_clients;i++) {
			if(client_sock_fd != client_sock[i]) {
				// loop_write(client_connections[i], &message, sizeof(message));
				loop_write(client_sock[i], buf, sizeof(buf));
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
	return 0; 
} 
