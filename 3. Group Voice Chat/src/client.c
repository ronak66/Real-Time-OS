// Client side C/C++ program to demonstrate Socket programming 
#include <stdio.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <unistd.h> 
#include <string.h> 
#include <signal.h>
#include <pthread.h>
#include <pulse/simple.h>
#include <pulse/error.h>
#include <errno.h>
#include <fcntl.h>
#include "../include/utils.h"   
// #define PORT 4141 
#define PORT 8080

#define BUFSIZE 1024

static const pa_sample_spec ss = {
    .format = PA_SAMPLE_S16LE,
    .rate = 44100,
    .channels = 2
};



int sock = 0;
char user_id[20],group_id[20];
char name[20];

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


void *send_message(){
	pa_simple *s = NULL;
    int ret = 1;
    int error;
    /* Create the recording stream */
    if (!(s = pa_simple_new(NULL, name, PA_STREAM_RECORD, NULL, "record", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__": pa_simple_new() failed: %s\n", pa_strerror(error));
        goto finish;
    }
    char key;
    printf("Enter any key to send voice message: ");
    scanf("%c",&key);
    int i=0;
    for (;;) {
		// struct Message message;
        // if(i==1000){
        //     printf("Enter any key to send voice message: ");
        //     scanf("%c",&key);
        //     i=0;
        // }
        uint8_t buf[BUFSIZE];
        struct Message message;
        strcpy(message.name,"ronak\0");
        /* Record some data ... */
        if (pa_simple_read(s, message.msg, sizeof(message.msg), &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_read() failed: %s\n", pa_strerror(error));
            goto finish;
        }
		// memcpy(message.msg, buf, sizeof(buf));
		// strcpy(message.name, name);
        /* And write it to STDOUT */
        if (loop_write(sock, &message, sizeof(message)) != sizeof(message)) {
            fprintf(stderr, __FILE__": write() failed: %s\n", strerror(errno));
            goto finish;
        }
        i++;
    }
    ret = 0;
finish:
    if (s)
        pa_simple_free(s);
}

void *recieve_message(){
	pa_simple *s = NULL;
    int ret = 1;
    int error;
    /* Create a new playback stream */
    if (!(s = pa_simple_new(NULL, name, PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error))) {
        fprintf(stderr, __FILE__ ": pa_simple_new() failed: %s\n", pa_strerror(error));
        goto finish;
    }
	for (;;) {
        uint8_t buf[BUFSIZE];
        struct Message message;
		// struct Message message;
        ssize_t r;
        /* Read some data ... */
        if ((r = read(sock, &message, sizeof(message))) <= 0) {
            if (r == 0) /* EOF */
                break;

            fprintf(stderr, __FILE__": read() failed: %s\n", strerror(errno));
            goto finish;
        }
		// memcpy(buf, message.msg, sizeof(buf));
		// printf("%s says : \n", message.name);
        /* ... and play it */
        if (pa_simple_write(s, message.msg, sizeof(message.msg), &error) < 0) {
            fprintf(stderr, __FILE__": pa_simple_write() failed: %s\n", pa_strerror(error));
            goto finish;
        }
    }
    /* Make sure that every single sample was played */
    if (pa_simple_drain(s, &error) < 0) {
        fprintf(stderr, __FILE__": pa_simple_drain() failed: %s\n", pa_strerror(error));
        goto finish;
    }
    ret = 0;
finish:
    if (s)
        pa_simple_free(s);

}

struct Init get_data(){
	printf("Input Name: ");
	scanf("%[^\n]%*c", name);
	printf("Input user id: ");
	scanf("%[^\n]%*c", user_id);
    printf("Input group id: ");
	scanf("%[^\n]%*c", group_id);
	struct Init data ;
	strcpy(data.group_id, group_id);
	strcpy(data.user_id, user_id);
	// data.number = number;
	printf("%s and %s and %s\n", name, user_id, group_id);
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
	// if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0) 	
    // if(inet_pton(AF_INET, "40.121.60.204", &serv_addr.sin_addr)<=0)  //nithin
    if(inet_pton(AF_INET, "52.149.151.135", &serv_addr.sin_addr)<=0) 	
	// if(inet_pton(AF_INET, "13.82.142.97", &serv_addr.sin_addr)<=0) 
	
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nConnection Failed \n"); 
		return -1; 
	}


	struct Init data;
	data = get_data();
	send(sock, &data, sizeof(data),0);

	pthread_create(&send_thread, NULL, send_message, NULL);
	pthread_create(&recv_thread, NULL, recieve_message, NULL);


	pthread_join(send_thread, NULL);
	pthread_join(recv_thread, NULL);
	return 0; 
} 
