#ifndef CLIENT_H_
#define CLIENT_H_

// from pa1_demo_code/client.c 
int connect_to_host(char *server_ip, char *server_port); 

int validate_ip(char* ip_addr);
int validate_port(char* port);

char* get_msg_from_buffer(char* buffer, int buffer_size, char delim, int nth); 

#endif // CLIENT_H_ 