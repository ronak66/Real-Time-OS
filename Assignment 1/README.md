# Real time Chat program

## Run Half Duplex Program
1. Compile Code  
```
$ gcc server.c -o server
$ gcc client.c -o client
```
2. Run Executable  
```
$ ./server
$ ./client
```


## Run Full Duplex Program
1. Compile Code
```
$ gcc server.c -o server -pthread
$ gcc client.c -o client -pthread
```
2. Run Executable
```
$ ./server
$ ./client
```

