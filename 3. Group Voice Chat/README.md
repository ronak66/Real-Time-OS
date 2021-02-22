# Group Voice Chat

## Run Program
1. Compile Code  
```
$ cd src/
$ gcc server.c -o server -pthread
$ gcc client.c -o client -pthread -lpulse -lpulse-simple
```
2. Run Executable  
```
$ ./server
$ ./client
```
Note: You can make as many clients as you want
