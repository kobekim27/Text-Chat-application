#include "../include/global.h" 
#include "../include/logger.h" 
#include "../include/linkedlist.h" 

// https://www.tutorialspoint.com/data_structures_algorithms/linked_list_program_in_c.htm 
#include <stdio.h> 
#include <string.h> 
#include <stdlib.h> 

void print_list(node** _list)
{
    node* list = *_list; 
    int list_id = 1; 
    for (; list; list = list->next, list_id = list_id + 1) {
       cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", list_id, list->hostname, list->ip_addr, list->port);
    }   // for (list)
}

void print_statistics(node** _list)
{
    node* list = *_list; 
    int list_id = 1; 
    for (; list; list = list->next, list_id = list_id + 1) {
        if (list->info->LOGGED_IN) {
            cse4589_print_and_log("%-5d%-35s%-8d%-8d%-8s\n", list_id, list->hostname, list->info->num_msg_sent, list->info->num_msg_rcv, "logged-in"); 
        }   // if (list->info->LOGGED_IN) 
        else { 
            cse4589_print_and_log("%-5d%-35s%-8d%-8d%-8s\n", list_id, list->hostname, list->info->num_msg_sent, list->info->num_msg_rcv, "logged-out"); 
        }   // else 
    }   // for (list) 
}

void print_blocked_list(node** _list, char* ip_addr)
{
    node* list = *_list; 
    int list_id = 1; 
    client_info* info = get_client_info_by_ip_address(_list, ip_addr); 

    for (; list; list = list->next) {
        if (check_ip_addr_blocked(info, list->ip_addr) == TRUE) {
            cse4589_print_and_log("%-5d%-35s%-20s%-8d\n", list_id, list->hostname, list->ip_addr, list->port);
            list_id = list_id + 1; 
        }   // if (check_ip_addr_blocked) 
    }   // for (list)
}

void insert_node(node** _list, char* hostname, char* ip_addr, int port, int sock_index, int server) 
{
    node* list = *_list; 
    node* prev = NULL; 
    node* new = (node*)malloc(sizeof(node)); 

    client_info* new_info = NULL; 

    strcpy(new->hostname, hostname);
    strcpy(new->ip_addr, ip_addr);
    new->port = port;
    new->sock_index = sock_index;
    new->next = NULL;

    if (server != FALSE) {
        new_info = (client_info*)malloc(sizeof(client_info)); 
        new_info->LOGGED_IN = TRUE;
        new_info->num_msg_sent = 0; 
        new_info->num_msg_rcv = 0; 

        new_info->blocked_by_num = 0; 
        new_info->blocked_by_ip = NULL; 

        new_info->msg_num = 0; 
        new_info->msg = NULL; 
    } // if (server) 

    new->info = new_info; 

    if (list == NULL) {
        *_list = new;
    }   // if (node*)
    else {
        for (; list && list->port <= port; prev = list, list = list->next);

        if (prev == NULL) {
            new->next = list;

            *_list = new;
        }   // if (prev) 
        else {
            new->next = prev->next;
            prev->next = new;
        }   // else 
    }   // else 
}

void delete_node(node** _list, int sock_index)
{
    node* list = *_list;
    node* prev = NULL; 

    for (; list && list->sock_index != sock_index; prev = list, list = list->next);

    if (list == NULL) {
        perror("the given socket index does not exist");
    }   // if (list) 

    if (prev == NULL) {
        *_list = list->next;
    }   // if (prev) 
    else {
        prev->next = list->next;
    }   // else 

    free(list);
}

void delete_list(node** _list)
{
    node* list = *_list;
    node* prev = NULL; 

    for (; list; prev = list, list = list->next, free(prev));

    *_list = NULL; 
}

int get_socket_index(node** _list, char* ip_addr)
{
    node* list = *_list; 

    for (; list; list = list->next) {
        if (strcmp(ip_addr, list->ip_addr) == 0) {
            return list->sock_index; 
        }   // if (strcmp())
    }   // for () 
    return -1; 
}

client_info* get_client_info_by_socket_index(node** _list, int sock_index)
{
    node* list = *_list; 

    for (; list; list = list->next) {
        if (list->sock_index == sock_index) {
            return list->info; 
        }   // if (list->sock_index)
    }   // for () 
    
    return NULL; 
}

client_info* get_client_info_by_ip_address(node** _list, char* ip_addr)
{
    node* list = *_list; 

    for (; list; list = list->next) {
        if (strcmp(list->ip_addr, ip_addr) == 0) {
            return list->info; 
        }   // if (list->sock_index)
    }   // for () 
    
    return NULL; 
}

char* get_ip_address(node** _list, int sock_index)
{
    node* list = *_list; 

    for (; list; list = list->next) {
        if (list->sock_index == sock_index) {
            return list->ip_addr; 
        }   // if (list->sock_index)
    }   // for () 
    
    return NULL; 
}

int check_ip_addr_blocked(client_info* info, char* ip_addr)
{
    ip_node* temp = NULL; 

    for (temp = info->blocked_by_ip; temp; temp = temp->next) {
        if (strcmp(temp->ip_addr, ip_addr) == 0) {
            return TRUE; 
        }   // if (strcmp) 
    }   // for (temp) 

    return FALSE; 
}

void insert_buffered_msg(client_info* info, char* from_ip_addr, char* to_ip_addr, char* msg)
{
    msg_node* msg_info = (msg_node*)malloc(sizeof(msg_node)); 
    msg_node* temp = NULL; 
    msg_node* prev = NULL; 
    strcpy(msg_info->ip_addr, from_ip_addr); 
    strcpy(msg_info->to_ip_addr, to_ip_addr); 
    memcpy(msg_info->msg, msg, 256); 
    msg_info->next = NULL; 

    if (info->msg_num == 0) {
        info->msg = msg_info; 
    }   // if (info->msg_num)
    else {
        for (temp = info->msg; temp; prev = temp, temp = temp->next); 

        prev->next = msg_info; 
    }   // else 

    info->msg_num = info->msg_num + 1; 
}

int insert_blocked_ip(client_info* info, char* ip_addr)
{
    ip_node* ip_info = (ip_node*)malloc(sizeof(ip_node)); 
    ip_node* temp = NULL; 
    ip_node* prev = NULL; 
    strcpy(ip_info->ip_addr, ip_addr); 
    ip_info->next = NULL; 

    if (info->blocked_by_num == 0) {
        info->blocked_by_ip = ip_info; 
    }   // if (info->blocked_by_num) 
    else {
        for (temp = info->blocked_by_ip; temp; prev = temp, temp = temp->next) {
            if (strcmp(temp->ip_addr, ip_addr) == 0) {
                free(ip_info); 
                return FALSE; 
            }   // if (strcmp())
        }   // for (temp) 

        prev->next = ip_info; 
    }   // else 

    info->blocked_by_num = info->blocked_by_num + 1; 
    
    return TRUE; 
}

int delete_blocked_ip(client_info* info, char* ip_addr)
{
    ip_node* temp = info->blocked_by_ip; 
    ip_node* prev = NULL; 

    for (; temp && (strcmp(temp->ip_addr, ip_addr) != 0); prev = temp, temp = temp->next);

    if (temp == NULL) {
        return FALSE; 
    }   // if (temp) 

    if (prev == NULL) {
        info->blocked_by_ip = temp->next;
    }   // if (prev) 
    else {
        prev->next = temp->next;
    }   // else 

    free(temp);

    info->blocked_by_num = info->blocked_by_num - 1; 

    return TRUE; 
}