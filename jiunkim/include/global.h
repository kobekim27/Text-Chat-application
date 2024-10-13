#ifndef GLOBAL_H_
#define GLOBAL_H_

// from pa1_demo_code/server.c 
#include <sys/socket.h>
#include <netinet/in.h>
#include <strings.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <netdb.h>

// from pa1_demo_code/client.c 
// #include <sys/socket.h>
// #include <strings.h>
// #include <string.h>
#include <arpa/inet.h>
// #include <sys/types.h>
// #include <netdb.h>

#include "client.h"
#include "linkedlist.h"
#include "ip.h"

#define HOSTNAME_LEN 128
#define PATH_LEN 256

// from pa1_demo_code/server.c 
#define BACKLOG 5
#define STDIN 0
#define TRUE 1
#define CMD_SIZE 100
#define BUFFER_SIZE 512

// from pa1_demo_code/client.c 
// #define TRUE 1
#define MSG_SIZE 512
// #define BUFFER_SIZE 256

#define FALSE 0

#endif
