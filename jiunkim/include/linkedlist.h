#ifndef LINKEDLIST_H_
#define LINKEDLIST_H_

typedef struct ip_node {
    char ip_addr[16]; 

    struct ip_node* next; 
} ip_node; 

typedef struct msg_node {
    char ip_addr[16];
    char to_ip_addr[16]; // for broadcast 
    char msg[256]; 

    struct msg_node* next; 
} msg_node; 

typedef struct client_info {
    int LOGGED_IN; 
    int num_msg_sent; 
    int num_msg_rcv; 

    int blocked_by_num; 
    struct ip_node* blocked_by_ip;

    char msg_num; 
    struct msg_node* msg;
} client_info;

// https://www.tutorialspoint.com/data_structures_algorithms/linked_list_program_in_c.htm 
typedef struct node {
    char hostname[256]; 
    char ip_addr[16];
    int port;
    int sock_index;
    struct client_info* info; 

    struct node* next;
} node; 

void print_list(node** _list);
void print_statistics(node** _list); 
void print_blocked_list(node** _list, char* ip_addr); 
void insert_node(node** _list, char* hostname, char* ip_addr, int port, int sock_index, int server);
void delete_node(node** _list, int sock_index);
void delete_list(node** _list);

int get_socket_index(node** _list, char* ip_addr);
client_info* get_client_info_by_socket_index(node** _list, int sock_index); 
client_info* get_client_info_by_ip_address(node** _list, char* ip_addr); 
char* get_ip_address(node** _list, int sock_index); 
int check_ip_addr_blocked(client_info* info, char* ip_addr); 
void insert_buffered_msg(client_info* info, char* from_ip_addr, char* to_ip_addr, char* msg); 
int insert_blocked_ip(client_info* info, char* ip_addr); 
int delete_blocked_ip(client_info* info, char* ip_addr); 

#endif // LINKEDLIST_H_ 