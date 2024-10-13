// from pa1_demo_code/client.c 
#include "../include/global.h" 
#include "../include/client.h" 

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

int connect_to_host(char *server_ip, char* server_port)
{
	int fdsocket;
	struct addrinfo hints, *res;

	/* Set up hints structure */	
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	/* Fill up address structures */	
	if (getaddrinfo(server_ip, server_port, &hints, &res) != 0) {
		perror("getaddrinfo failed");
		freeaddrinfo(res);
		return -1;
	}	// if (getaddrinf())

	/* Socket */
	fdsocket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	if(fdsocket < 0) {
		perror("Failed to create socket");
		freeaddrinfo(res);
		return -1;
	}	// if (fdsocket()) 
	
	/* Connect */
	if(connect(fdsocket, res->ai_addr, res->ai_addrlen) < 0) {
		perror("Connect failed");
		freeaddrinfo(res);
		return -1;
	}	// if (connect())
	
	freeaddrinfo(res);

	return fdsocket;
}

int validate_ip(char* ip_addr)
{
    char* _ip_addr = NULL; 
    char* token = NULL; 
    int i = 0; 
    int period_num = -1;
    char* saveptr = NULL; 

    _ip_addr = (char*)malloc(sizeof(char) * (strlen(ip_addr) + 1)); 
    strcpy(_ip_addr, ip_addr);
    token = strtok_r(_ip_addr, ".", &saveptr);

    if (strlen(ip_addr) > 15) {
        return FALSE;
    }   // if (strlen())
    
    while (token) {
        for (i = 0; i < strlen(token); i++) {
            if (isdigit((token[i])) == 0) {
                return FALSE;
            }   // if (isdigit())
        }   // for (i) 
        if (atoi(token) < 0 || atoi(token) > 255) {
            return FALSE;
        }   // if (atoi(token))

        period_num++; 
        token = strtok_r(NULL, ".", &saveptr);
    }   // while (token)
    
    if (period_num != 3) {
        return FALSE;
    }   // if (period_num)
    
    free(_ip_addr);
    return TRUE;
}

int validate_port(char* port)
{
    int i = 0; 
    int port_num = 0; 

    for (i = 0; i < strlen(port); i++) {
        if (isdigit(port[i]) == 0) {
            return FALSE;
        }   // if (isdigit())
    }   // for (i) 

    port_num = atoi(port); 

    if (port_num < 0 || port_num > 65535) {
        return FALSE;
    }   // if (port_num) 

    return TRUE; 
}

char* get_msg_from_buffer(char* buffer, int buffer_size, char delim, int nth)
{
    char* msg = NULL; 
    int i = 0; 
    int j = 0; 
    int k = 0; 

    for (i = 0; i < buffer_size; i++) {
        if (buffer[i] == delim) {
            j = j + 1; 
            if (j == nth) break; 
        }   // if (buffer[i]) 
    }   // for (i) 

    msg = (char*)malloc(256 * sizeof(char)); 
    memset(msg, '\0', 256); 
    for (i = i + 1; (i < buffer_size) && (k < 256); k++, i++) {
        msg[k] = buffer[i]; 
    }   // for (i) 

    return msg; 
}