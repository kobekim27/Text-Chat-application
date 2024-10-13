/**
 * @jiunkim_assignment1
 * @author  Changjae Lee <changjae@buffalo.edu>, Jiun Kim <jiunkim@buffalo.edu>
 * @version 1.0
 *
 * @section LICENSE
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details at
 * http://www.gnu.org/copyleft/gpl.html
 *
 * @section DESCRIPTION
 *
 * This contains the main function. Add further description here....
 */
#include <stdio.h>
#include <stdlib.h>

#include "../include/global.h"
#include "../include/logger.h"

/**
 * main function
 *
 * @param  argc Number of arguments
 * @param  argv The argument list
 * @return 0 EXIT_SUCCESS
 */
int main(int argc, char **argv)
{
	/*Init. Logger*/
	cse4589_init_log(argv[2]);

	/*Clear LOGFILE*/
	fclose(fopen(LOGFILE, "w"));

	/*Start Here*/
	int listening_port = 0;

	// printf("argc: %d\n", argc); // includes ./assignment1
	if (argc != 3) {
		perror("The number of given arguments is not 2.\n");
		exit(-1); 
	}	// if (argc) 

	listening_port = atoi(argv[2]);
	node* list = NULL;
	int client_num = 0;
	int logged_in_client_num = 0; 
	char ip_addr[16] = "";
	int i = 0; 
	const char* delim = "[ACK]";
	char* token = NULL; 
	// https://stackoverflow.com/questions/15961253/c-correct-usage-of-strtok-r 
	char* saveptr = NULL; 

	int sock_index_for_client_to_receive = 0; 

	fd_set master_list, watch_list;
	int head_socket, selret, sock_index;

	/* Zero select FD sets */
	FD_ZERO(&master_list);
	FD_ZERO(&watch_list);

	if (get_ip(ip_addr) != TRUE) {
		perror("get ip address failed");
	}	// if get_ip() 

	if (argv[1][0] == 's') {
		// from pa1_demo_code/server.c 
		int server_socket, fdaccept=0, caddr_len;
		struct sockaddr_in client_addr;
		struct addrinfo hints, *res;

		/* Set up hints structure */
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = AF_INET;
		hints.ai_socktype = SOCK_STREAM;
		hints.ai_flags = AI_PASSIVE;

		/* Fill up address structures */
		// argv[1] -> argv[2] 
		if (getaddrinfo(NULL, argv[2], &hints, &res) != 0)
			perror("getaddrinfo failed");
		
		/* Socket */
		server_socket = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(server_socket < 0)
			perror("Cannot create socket");
		
		/* Bind */
		if(bind(server_socket, res->ai_addr, res->ai_addrlen) < 0 )
			perror("Bind failed");

		freeaddrinfo(res);
		
		/* Listen */
		if(listen(server_socket, BACKLOG) < 0)
			perror("Unable to listen on port");

		/* ---------------------------------------------------------------------------- */
		/* Register the listening socket */
		FD_SET(server_socket, &master_list);
		/* Register STDIN */
		FD_SET(STDIN, &master_list);
		
		head_socket = server_socket;
		
		while(TRUE){
			memcpy(&watch_list, &master_list, sizeof(master_list));
			
			/* select() system call. This will BLOCK */
			selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
			if(selret < 0)
				perror("select failed.");
			
			/* Check if we have sockets/STDIN to process */
			if(selret > 0){
				/* Loop through socket descriptors to check which ones are ready */
				for(sock_index=0; sock_index<=head_socket; sock_index+=1){
					
					if(FD_ISSET(sock_index, &watch_list)){
						
						/* Check if new command on STDIN */
						if (sock_index == STDIN){
							char *cmd = (char*) malloc(sizeof(char)*CMD_SIZE);
							
							memset(cmd, '\0', CMD_SIZE);
							if(fgets(cmd, CMD_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to cmd
								exit(-1);
							
							// Remove the newline character in 'cmd' char array 
							// https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input 
							cmd[strcspn(cmd, "\n")] = 0;
							cmd = strtok(cmd, " ");
							
							//Process PA1 commands here ...
							if (strcmp(cmd, "AUTHOR") == 0) {
								cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
								cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "changjae"); 
								cse4589_print_and_log("[%s:END]\n", cmd);
							}	// if (strcmp(cmd)) 
							else if (strcmp(cmd, "IP") == 0) {
								if (get_ip(ip_addr) == TRUE) {
									// Print the IP address 
									cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
									cse4589_print_and_log("IP:%s\n", ip_addr); 
									cse4589_print_and_log("[%s:END]\n", cmd); 
								}	// if (get_ip())
								else {
									cse4589_print_and_log("[%s:ERROR]\n", cmd);
									cse4589_print_and_log("[%s:END]\n", cmd); 
								}	// else 
							}	// else if (strcmp(cmd)) 
							else if (strcmp(cmd, "PORT") == 0) {
								// Print the PORT 
								cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
								cse4589_print_and_log("PORT:%d\n", listening_port); 
								cse4589_print_and_log("[%s:END]\n", cmd); 
							}	// else if (strcmp(cmd)) 
							else if (strcmp(cmd, "LIST") == 0) {
								cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
								print_list(&list);
								cse4589_print_and_log("[%s:END]\n", cmd); 
							}	// else if (strcmp(cmd)) 
							else if (strcmp(cmd, "STATISTICS") == 0) {
								cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
								print_statistics(&list);
								cse4589_print_and_log("[%s:END]\n", cmd); 
							}	// else if (strcmp(cmd)) 
							else if (strcmp(cmd, "BLOCKED") == 0) {
								char* client_ip = strtok(NULL, " "); 

								if (validate_ip(client_ip) && (get_socket_index(&list, client_ip) != -1)) {
									cse4589_print_and_log("[%s:SUCCESS]\n", cmd); 
									print_blocked_list(&list, client_ip); 
									cse4589_print_and_log("[%s:END]\n", cmd); 
								}	// if (validate_ip(client_ip) && (get_socket_index(&list, client_ip)))
								else {
									cse4589_print_and_log("[%s:ERROR]\n", cmd);
									cse4589_print_and_log("[%s:END]\n", cmd); 
								}	// else 
							}	// else if (strcmp(cmd)) 
							else {
								perror("Not supported!\n");
							}	// else 
							free(cmd); 
						}	// if (sock_index) 
						/* Check if new client is requesting connection */
						else if(sock_index == server_socket){
							caddr_len = sizeof(client_addr);
							fdaccept = accept(server_socket, (struct sockaddr *)&client_addr, &caddr_len);
							if(fdaccept < 0)
								perror("Accept failed.");
							
							/* Add to watched socket list */
							FD_SET(fdaccept, &master_list);
							if(fdaccept > head_socket) head_socket = fdaccept;

							// Receive the message from the new client. 
							// The next client message contains hostname, ip address, and port. 
							char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
							memset(buffer, '\0', BUFFER_SIZE);

							if(recv(fdaccept, buffer, BUFFER_SIZE, 0) <= 0){
								close(fdaccept);
								/* Remove from watched list */
								FD_CLR(fdaccept, &master_list);
								delete_node(&list, fdaccept);
								client_num = client_num - 1;
								logged_in_client_num = logged_in_client_num - 1; 
							}	// if (recv()) 
							else {
								char* client_hostname = strtok(buffer, " ");
								char* client_ip_addr = strtok(NULL, " ");
								int client_port = atoi(strtok(NULL, delim));
								insert_node(&list, client_hostname, client_ip_addr, client_port, fdaccept, TRUE);
								client_num = client_num + 1;
								logged_in_client_num = logged_in_client_num + 1; 

								memset(buffer, '\0', BUFFER_SIZE);
								sprintf(buffer, "%d%s", logged_in_client_num, delim);
								if (send(fdaccept, buffer, strlen(buffer), 0) != strlen(buffer)) {
									perror("send() failed");
								}	// if (send()) 

								memset(buffer, '\0', BUFFER_SIZE);
								if (recv(fdaccept, buffer, BUFFER_SIZE, 0) >= 0){
									if (strcmp(buffer, delim) != 0) {
										perror("Client did not send ACK");
									}	// if (strcmp())
									for (node* temp = list; temp; temp = temp->next) {
										if (temp->info->LOGGED_IN == TRUE) {
											memset(buffer, '\0', BUFFER_SIZE);
											sprintf(buffer, "%s %s %d %d%s", temp->hostname, temp->ip_addr, temp->port, temp->sock_index, delim);

											if (send(fdaccept, buffer, strlen(buffer), 0) != strlen(buffer)) {
												perror("send() failed");
											}	// if (send()) 
										}	// if (temp->info->LOGGED_IN) 
									}	// for (temp)
								}	// if (recv())

								fflush(stdout);
							}	// else 
							
							free(buffer);

						}	// else if (sock_index) 
						/* Read from existing clients */
						else{
							/* Initialize buffer to receieve response */
							char *buffer = (char*) malloc(sizeof(char)*BUFFER_SIZE);
							memset(buffer, '\0', BUFFER_SIZE);
							
							if(recv(sock_index, buffer, BUFFER_SIZE, 0) <= 0){
								close(sock_index);
								/* Remove from watched list */
								FD_CLR(sock_index, &master_list);
								delete_node(&list, sock_index);
								client_num = client_num - 1;
								logged_in_client_num = logged_in_client_num - 1; 
							}	// if (recv()) 
							else {
								//Process incoming data from existing clients here ...
								if (strcmp(buffer, "REFRESH") == 0) {
									memset(buffer, '\0', BUFFER_SIZE);
									sprintf(buffer, "%d%s", logged_in_client_num, delim);
									if (send(sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
										perror("send() failed");
									}	// if (send()) 

									memset(buffer, '\0', BUFFER_SIZE);
									if (recv(sock_index, buffer, BUFFER_SIZE, 0) >= 0){
										if (strcmp(buffer, delim) != 0) {
											perror("Client did not send ACK");
										}	// if (strcmp())
										for (node* temp = list; temp; temp = temp->next) {
											if (temp->info->LOGGED_IN == TRUE) {
												memset(buffer, '\0', BUFFER_SIZE);
												sprintf(buffer, "%s %s %d %d%s", temp->hostname, temp->ip_addr, temp->port, temp->sock_index, delim);

												if (send(sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
													perror("send() failed");
												}	// if (send()) 
											}	// if (temp->info->LOGGED_IN) 
										}	// for (temp)
									}	// if (recv())

									fflush(stdout);
								}	// if (strcmp(buffer))
								else if (strcmp(buffer, "LOGIN") == 0) {
									client_info* current_client_info = get_client_info_by_socket_index(&list, sock_index);
									current_client_info->LOGGED_IN = TRUE; 
									logged_in_client_num = logged_in_client_num + 1; 
									memset(buffer, '\0', BUFFER_SIZE);
									sprintf(buffer, "%d%s", logged_in_client_num, delim);
									if (send(sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
										perror("send() failed");
									}	// if (send()) 

									memset(buffer, '\0', BUFFER_SIZE);
									if (recv(sock_index, buffer, BUFFER_SIZE, 0) >= 0){
										if (strcmp(buffer, delim) != 0) {
											perror("Client did not send ACK");
										}	// if (strcmp())
										for (node* temp = list; temp; temp = temp->next) {
											if (temp->info->LOGGED_IN == TRUE) {
												memset(buffer, '\0', BUFFER_SIZE);
												sprintf(buffer, "%s %s %d %d%s", temp->hostname, temp->ip_addr, temp->port, temp->sock_index, delim);

												if (send(sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
													perror("send() failed");
												}	// if (send()) 
											}	// if (temp->info->LOGGED_IN) 
										}	// for (temp)
									}	// if (recv())
									
									memset(buffer, '\0', BUFFER_SIZE);
									if (recv(sock_index, buffer, BUFFER_SIZE, 0) >= 0){
										if (strcmp(buffer, delim) != 0) {
											perror("Client did not send ACK");
										}	// if (strcmp())
									}	// if (recv()) 

									memset(buffer, '\0', BUFFER_SIZE);
									sprintf(buffer, "%d%s", current_client_info->msg_num, delim);
									if (send(sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
										perror("send() failed");
									}	// if (send()) 

									memset(buffer, '\0', BUFFER_SIZE);
									if (recv(sock_index, buffer, BUFFER_SIZE, 0) >= 0){
										if (strcmp(buffer, delim) != 0) {
											perror("Client did not send ACK");
										}	// if (strcmp())
									}	// if (recv()) 

									for (i = 0; i < current_client_info->msg_num; i++) {
										memset(buffer, '\0', BUFFER_SIZE);
										sprintf(buffer, "%s", current_client_info->msg->ip_addr);
										
										if (send(sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
											perror("send() failed");
										}	// if (send()) 

										memset(buffer, '\0', BUFFER_SIZE);
										if (recv(sock_index, buffer, BUFFER_SIZE, 0) >= 0){
											if (strcmp(buffer, delim) != 0) {
												perror("Client did not send ACK");
											}	// if (strcmp())
										}	// if (recv()) 

										memset(buffer, '\0', BUFFER_SIZE);
										sprintf(buffer, "%s", current_client_info->msg->msg);
										
										if (send(sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
											perror("send() failed");
										}	// if (send()) 

										memset(buffer, '\0', BUFFER_SIZE);
										if (recv(sock_index, buffer, BUFFER_SIZE, 0) >= 0){
											if (strcmp(buffer, delim) != 0) {
												perror("Client did not send ACK");
											}	// if (strcmp())
										}	// if (recv()) 

										cse4589_print_and_log("[RELAYED:SUCCESS]\n");
										cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n", current_client_info->msg->ip_addr, current_client_info->msg->to_ip_addr, current_client_info->msg->msg);
										cse4589_print_and_log("[RELAYED:END]\n"); 

										current_client_info->msg = current_client_info->msg->next; 

									}	// for (i) 
									
									current_client_info->num_msg_rcv = current_client_info->num_msg_rcv + current_client_info->msg_num; 
									current_client_info->msg_num = 0; 
									current_client_info->msg = NULL; 

									fflush(stdout);
								}	// else if (strcmp(buffer)) 
								else if (strcmp(buffer, "LOGOUT") == 0) {
									client_info* current_client_info = get_client_info_by_socket_index(&list, sock_index); 
									current_client_info->LOGGED_IN = FALSE; 
									logged_in_client_num = logged_in_client_num - 1; 
									// send acknowldegment 
									memset(buffer, '\0', BUFFER_SIZE); 
									sprintf(buffer, "%s", delim); 
									if (send(sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
										perror("send() failed");
									}	// if (send()) 
									
									fflush(stdout);
								}	// else if (strcmp(buffer)) 
								// You can assume that the total number of buffered messages will not exceed 100. 
								else if (strcmp(buffer, "SEND") == 0) {
									// send acknowldegment 
									memset(buffer, '\0', BUFFER_SIZE); 
									sprintf(buffer, "%s", delim); 
									if (send(sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
										perror("send() failed");
									}	// if (send()) 

									// receive the client_ip to send the message and get the corresponding socket index 
									memset(buffer, '\0', BUFFER_SIZE);
									if (recv(sock_index, buffer, BUFFER_SIZE, 0) >= 0){
										sock_index_for_client_to_receive = get_socket_index(&list, buffer);
									}	// if (recv()) 

									// send acknowldgement 
									memset(buffer, '\0', BUFFER_SIZE); 
									sprintf(buffer, "%s", delim); 
									if (send(sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
										perror("send() failed");
									}	// if (send()) 

									memset(buffer, '\0', BUFFER_SIZE);
									if (recv(sock_index, buffer, BUFFER_SIZE, 0) < 0){
										perror("recv() failed");
									}	// if (recv())

									client_info* sender_client_info = get_client_info_by_socket_index(&list, sock_index);
									sender_client_info->num_msg_sent = sender_client_info->num_msg_sent + 1; 

									if (sock_index_for_client_to_receive != -1) {
										// receive the message and send the received message to client_ip 
										// The blocked sender, however, 
										// will be unaware of this blocking and should execute the SEND command without any error. 
										char* sender_client_ip_addr = get_ip_address(&list, sock_index); 
										client_info* receiver_client_info = get_client_info_by_socket_index(&list, sock_index_for_client_to_receive); 
										int blocked = check_ip_addr_blocked(receiver_client_info, sender_client_ip_addr); 
										if (blocked == FALSE) {
											char* receiver_client_ip_addr = get_ip_address(&list, sock_index_for_client_to_receive); 
											char* msg_to_send = (char*)malloc(256 * sizeof(char)); 
											memset(msg_to_send, '\0', 256); 
											memcpy(msg_to_send, buffer, 256); 
											if (receiver_client_info->LOGGED_IN == TRUE) {
												memset(buffer, '\0', BUFFER_SIZE);
												sprintf(buffer, "%s", "SEND");
												if (send(sock_index_for_client_to_receive, buffer, strlen(buffer), 0) != strlen(buffer)) {
													perror("send() failed");
												}	// if (send()) 

												memset(buffer, '\0', BUFFER_SIZE);
												if (recv(sock_index_for_client_to_receive, buffer, BUFFER_SIZE, 0) >= 0){
													if (strcmp(buffer, delim) != 0) {
														perror("Client did not send ACK");
													}	// if (strcmp())
												}	// if (recv()) 

												memset(buffer, '\0', BUFFER_SIZE);
												sprintf(buffer, "%s", sender_client_ip_addr);
												if (send(sock_index_for_client_to_receive, buffer, strlen(buffer), 0) != strlen(buffer)) {
													perror("send() failed");
												}	// if (send()) 

												memset(buffer, '\0', BUFFER_SIZE);
												if (recv(sock_index_for_client_to_receive, buffer, BUFFER_SIZE, 0) >= 0){
													if (strcmp(buffer, delim) != 0) {
														perror("Client did not send ACK");
													}	// if (strcmp())
												}	// if (recv()) 

												memset(buffer, '\0', BUFFER_SIZE);
												sprintf(buffer, "%s", msg_to_send);
												if (send(sock_index_for_client_to_receive, buffer, strlen(buffer), 0) != strlen(buffer)) {
													perror("send() failed");
												}	// if (send()) 

												memset(buffer, '\0', BUFFER_SIZE);
												if (recv(sock_index_for_client_to_receive, buffer, BUFFER_SIZE, 0) >= 0){
													if (strcmp(buffer, delim) != 0) {
														perror("Client did not send ACK");
													}	// if (strcmp())
												}	// if (recv()) 

												receiver_client_info->num_msg_rcv = receiver_client_info->num_msg_rcv + 1; 

												cse4589_print_and_log("[RELAYED:SUCCESS]\n");
												cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n", sender_client_ip_addr, receiver_client_ip_addr, msg_to_send);
												cse4589_print_and_log("[RELAYED:END]\n"); 
											}	// if (receiver_client_info->LOGGED_IN) 
											else {
												insert_buffered_msg(receiver_client_info, sender_client_ip_addr, receiver_client_ip_addr, msg_to_send); 
											}	// else 
											free(msg_to_send); 
										}	// if (blocked) 
										
										// send acknowldgement 
										memset(buffer, '\0', BUFFER_SIZE); 
										sprintf(buffer, "%s", delim); 
										if (send(sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
											perror("send() failed");
										}	// if (send()) 

									}	// if (sock_index_for_client_to_receive) 
									else {
										// The IP, to which sender client wants to send msg, does not exist in the list. 
										memset(buffer, '\0', BUFFER_SIZE); 
										sprintf(buffer, "NOT%s", delim); 
										if (send(sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
											perror("send() failed");
										}	// if (send()) 
									}	// else 

								}	// else if (strcmp(buffer)) 
								else if (strcmp(buffer, "BROADCAST") == 0) {
									// send acknowldegment 
									memset(buffer, '\0', BUFFER_SIZE); 
									sprintf(buffer, "%s", delim); 
									if (send(sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
										perror("send() failed");
									}	// if (send()) 

									memset(buffer, '\0', BUFFER_SIZE);
									if (recv(sock_index, buffer, BUFFER_SIZE, 0) < 0){
										perror("recv() failed");
									}	// if (recv())

									char* msg_to_send = (char*)malloc(256 * sizeof(char)); 
									memset(msg_to_send, '\0', 256); 
									memcpy(msg_to_send, buffer, 256); 
									client_info* sender_client_info = get_client_info_by_socket_index(&list, sock_index);
									sender_client_info->num_msg_sent = sender_client_info->num_msg_sent + 1; 

									char* sender_client_ip_addr = get_ip_address(&list, sock_index); 

									for (node* temp = list; temp; temp = temp->next) {
										if (temp->sock_index != sock_index) {
											client_info* receiver_client_info = get_client_info_by_socket_index(&list, temp->sock_index); 
											int blocked = check_ip_addr_blocked(receiver_client_info, sender_client_ip_addr); 
											if (blocked == FALSE) {
												if (receiver_client_info->LOGGED_IN == TRUE) {
													memset(buffer, '\0', BUFFER_SIZE);
													sprintf(buffer, "%s", "SEND");
													if (send(temp->sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
														perror("send() failed");
													}	// if (send()) 

													memset(buffer, '\0', BUFFER_SIZE);
													if (recv(temp->sock_index, buffer, BUFFER_SIZE, 0) >= 0){
														if (strcmp(buffer, delim) != 0) {
															perror("Client did not send ACK");
														}	// if (strcmp())
													}	// if (recv()) 

													memset(buffer, '\0', BUFFER_SIZE);
													sprintf(buffer, "%s", sender_client_ip_addr);
													if (send(temp->sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
														perror("send() failed");
													}	// if (send()) 

													memset(buffer, '\0', BUFFER_SIZE);
													if (recv(temp->sock_index, buffer, BUFFER_SIZE, 0) >= 0){
														if (strcmp(buffer, delim) != 0) {
															perror("Client did not send ACK");
														}	// if (strcmp())
													}	// if (recv()) 

													memset(buffer, '\0', BUFFER_SIZE);
													sprintf(buffer, "%s", msg_to_send);
													if (send(temp->sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
														perror("send() failed");
													}	// if (send()) 

													memset(buffer, '\0', BUFFER_SIZE);
													if (recv(temp->sock_index, buffer, BUFFER_SIZE, 0) >= 0){
														if (strcmp(buffer, delim) != 0) {
															perror("Client did not send ACK");
														}	// if (strcmp())
													}	// if (recv()) 

													receiver_client_info->num_msg_rcv = receiver_client_info->num_msg_rcv + 1; 

													cse4589_print_and_log("[RELAYED:SUCCESS]\n");
													cse4589_print_and_log("msg from:%s, to:%s\n[msg]:%s\n", sender_client_ip_addr, "255.255.255.255", msg_to_send);
													cse4589_print_and_log("[RELAYED:END]\n"); 
												}	// if (receiver_client_info->LOGGED_IN) 
												else {
													insert_buffered_msg(receiver_client_info, sender_client_ip_addr, "255.255.255.255", msg_to_send); 
												}
											}	// if (blocked) 
										}	// if (temp->sock_index) 
									}	// for (temp) 

									memset(buffer, '\0', BUFFER_SIZE); 
									sprintf(buffer, "%s", delim); 
									if (send(sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
										perror("send() failed");
									}	// if (send()) 
									free(msg_to_send); 
								}	// else if (strcmp(buffer)) 
								else if (strcmp(buffer, "BLOCK") == 0) {
									// send acknowldegment 
									memset(buffer, '\0', BUFFER_SIZE); 
									sprintf(buffer, "%s", delim); 
									if (send(sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
										perror("send() failed");
									}	// if (send()) 

									// receive the client_ip to send the message and get the corresponding socket index 
									memset(buffer, '\0', BUFFER_SIZE);
									if (recv(sock_index, buffer, BUFFER_SIZE, 0) >= 0){
										client_info* sender_client_info = get_client_info_by_socket_index(&list, sock_index); 
										if (insert_blocked_ip(sender_client_info, buffer) == TRUE) {
											// send acknowldgement 
											memset(buffer, '\0', BUFFER_SIZE); 
											sprintf(buffer, "%s", delim); 
											if (send(sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
												perror("send() failed");
											}	// if (send()) 
										}	// if (insert_blocked_ip) 
										else {
											memset(buffer, '\0', BUFFER_SIZE); 
											sprintf(buffer, "NOT%s", delim); 
											if (send(sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
												perror("send() failed");
											}	// if (send()) 
										}	// else 
									}	// if (recv()) 
								}	// else if (strcmp(buffer)) 
								else if (strcmp(buffer, "UNBLOCK") == 0) {
									// send acknowldegment 
									memset(buffer, '\0', BUFFER_SIZE); 
									sprintf(buffer, "%s", delim); 
									if (send(sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
										perror("send() failed");
									}	// if (send()) 

									// receive the client_ip to send the message and get the corresponding socket index 
									memset(buffer, '\0', BUFFER_SIZE);
									if (recv(sock_index, buffer, BUFFER_SIZE, 0) >= 0){
										client_info* sender_client_info = get_client_info_by_socket_index(&list, sock_index); 
										if (delete_blocked_ip(sender_client_info, buffer) == TRUE) {
											// send acknowldgement 
											memset(buffer, '\0', BUFFER_SIZE); 
											sprintf(buffer, "%s", delim); 
											if (send(sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
												perror("send() failed");
											}	// if (send()) 
										}	// if (insert_blocked_ip) 
										else {
											memset(buffer, '\0', BUFFER_SIZE); 
											sprintf(buffer, "NOT%s", delim); 
											if (send(sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
												perror("send() failed");
											}	// if (send()) 
										}	// else 
									}	// if (recv()) 
								}	// else if (strcmp(buffer)) 

								fflush(stdout);
							}	// else 
							
							free(buffer);
						}	// else 
					}	// if (FD_ISSET()) 
				}	// for (sock_index) 
			}	// if (selret) 
		}	// while (TRUE) 

	}	// if (argv[1][0]) 
	else if (argv[1][0] == 'c') {
		int LOGIN_FLAG = FALSE;
		int CONNECTED = FALSE; 
		int buffered_msg_num = 0; 
		
		// from pa1_demo_code/client.c 
		int server;
		// argv[1], argv[2] -> argv[2], argv[3] 
		// https://www.geeksforgeeks.org/c-program-display-hostname-ip-address/ 
		char hostname[BUFFER_SIZE] = "";
		
		/* Register STDIN */
		FD_SET(STDIN, &master_list);

		head_socket = STDIN; 

		if (gethostname(hostname, sizeof(hostname)) == -1) {
			perror("gethostname failed\n");
		}	// if (gethostname())
		
		while(TRUE){
			memcpy(&watch_list, &master_list, sizeof(master_list)); 
			/* select() system call. This will BLOCK */
			selret = select(head_socket + 1, &watch_list, NULL, NULL, NULL);
			if(selret < 0)
				perror("select failed.");
			
			if (selret > 0) {
				for(sock_index=0; sock_index<=head_socket; sock_index+=1){
					if(FD_ISSET(sock_index, &watch_list)){
						/* Check if new command on STDIN */
						if (sock_index == STDIN){
							char *msg = (char*) malloc(sizeof(char)*MSG_SIZE);
							memset(msg, '\0', MSG_SIZE);
							if(fgets(msg, MSG_SIZE-1, stdin) == NULL) //Mind the newline character that will be written to msg
								exit(-1);
							
							// Remove the newline character in 'msg' char array 
							// https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input 
							msg[strcspn(msg, "\n")] = '\0';
							// Extract the first word from msg. 
							// https://www.educative.io/answers/splitting-a-string-using-strtok-in-c 
							char* cmd = (char*)malloc(MSG_SIZE*(sizeof(char))); 
							memset(cmd, '\0', MSG_SIZE); 
							memcpy(cmd, msg, MSG_SIZE); 
							cmd = strtok(cmd, " "); 
							
							//Process PA1 commands here ...
							if (strcmp(cmd, "AUTHOR") == 0) {
								cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
								cse4589_print_and_log("I, %s, have read and understood the course academic integrity policy.\n", "changjae"); 
								cse4589_print_and_log("[%s:END]\n", cmd);
							}	// if (strcmp(cmd)) 
							else if (strcmp(cmd, "IP") == 0) {
								if (get_ip(ip_addr) == TRUE) {
									// Print the IP address 
									cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
									cse4589_print_and_log("IP:%s\n", ip_addr); 
									cse4589_print_and_log("[%s:END]\n", cmd); 
								}	// if (get_ip())
								else {
									cse4589_print_and_log("[%s:ERROR]\n", cmd);
									cse4589_print_and_log("[%s:END]\n", cmd); 
								}	// else 
							}	// else if (strcmp(cmd)) 
							else if (strcmp(cmd, "PORT") == 0) {
								// Print the PORT 
								cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
								cse4589_print_and_log("PORT:%d\n", listening_port); 
								cse4589_print_and_log("[%s:END]\n", cmd); 
							}	// else if (strcmp(cmd)) 
							else if (strcmp(cmd, "LIST") == 0) {
								if (LOGIN_FLAG) {
									cse4589_print_and_log("[%s:SUCCESS]\n", cmd);
									print_list(&list);
									cse4589_print_and_log("[%s:END]\n", cmd); 
								}	// if (LOGIN_FLAG) 
							}	// else if (strcmp(cmd)) 
							else if (strcmp(cmd, "LOGIN") == 0) {
								char* server_ip = strtok(NULL, " "); 
								char* server_port = strtok(NULL, " "); 

								if (validate_ip(server_ip) && validate_port(server_port)) {
									if (CONNECTED == FALSE) {
										server = connect_to_host(server_ip, server_port); 
										if (server != -1) {
											LOGIN_FLAG = TRUE;
											CONNECTED = TRUE; 
											FD_SET(server, &master_list);
											if(server > head_socket) head_socket = server; 

											memset(msg, '\0', MSG_SIZE); 
											// https://www.tutorialspoint.com/c_standard_library/c_function_sprintf.htm 
											sprintf(msg, "%s %s %d%s", hostname, ip_addr, listening_port, delim);
											if (send(server, msg, strlen(msg), 0) != strlen(msg)) {
												perror("send() failed");
											}	// if (send()) 

											memset(msg, '\0', MSG_SIZE);
											if (recv(server, msg, MSG_SIZE, 0) >= 0) {
												client_num = atoi(strtok(msg, delim));
											}	// if (recv())

											memset(msg, '\0', MSG_SIZE);
											sprintf(msg, "%s", delim);
											if (send(server, msg, strlen(msg), 0) != strlen(msg)) {
												perror("send() failed");
											}	// if (send()) 

											i = 0;
											memset(msg, '\0', MSG_SIZE);
											while (recv(server, msg, MSG_SIZE, 0) >= 0) {
												// https://stackoverflow.com/questions/15961253/c-correct-usage-of-strtok-r 
												token = strtok_r(msg, delim, &saveptr); 

												while (token) {
													char* client_hostname = strtok(token, " ");
													char* client_ip_addr = strtok(NULL, " ");
													int client_port = atoi(strtok(NULL, " "));
													int client_sock_index = atoi(strtok(NULL, " "));
													insert_node(&list, client_hostname, client_ip_addr, client_port, client_sock_index, FALSE);

													i++; 
													token = strtok_r(NULL, delim, &saveptr);
												}	// while (token) 

												fflush(stdout);
												memset(msg, '\0', MSG_SIZE);
												if (i >= client_num) {
													break;
												}	// if (i) 
											}	// while (recv())
										}	// if (server) 
									}	// if (CONNECTED)
									else {
										memset(msg, '\0', MSG_SIZE);
										sprintf(msg, "%s", "LOGIN");
										if (send(server, msg, strlen(msg), 0) != strlen(msg)) {
											perror("send() failed");
										}	// if (send()) 

										delete_list(&list);

										memset(msg, '\0', MSG_SIZE);
										if (recv(server, msg, MSG_SIZE, 0) >= 0) {
											client_num = atoi(strtok(msg, delim));
										}	// if (recv())
										
										LOGIN_FLAG = TRUE; 

										memset(msg, '\0', MSG_SIZE);
										sprintf(msg, "%s", delim);
										if (send(server, msg, strlen(msg), 0) != strlen(msg)) {
											perror("send() failed");
										}	// if (send()) 

										i = 0;
										memset(msg, '\0', MSG_SIZE);
										while (recv(server, msg, MSG_SIZE, 0) >= 0) {
											// https://stackoverflow.com/questions/15961253/c-correct-usage-of-strtok-r 
											token = strtok_r(msg, delim, &saveptr); 

											while (token) {
												char* client_hostname = strtok(token, " ");
												char* client_ip_addr = strtok(NULL, " ");
												int client_port = atoi(strtok(NULL, " "));
												int client_sock_index = atoi(strtok(NULL, " "));
												insert_node(&list, client_hostname, client_ip_addr, client_port, client_sock_index, FALSE);

												i++; 
												token = strtok_r(NULL, delim, &saveptr);
											}	// while (token) 

											fflush(stdout);
											memset(msg, '\0', MSG_SIZE);
											if (i >= client_num) {
												break;
											}	// if (i) 
										}	// while (recv())

										memset(msg, '\0', MSG_SIZE);
										sprintf(msg, "%s", delim);
										if (send(server, msg, strlen(msg), 0) != strlen(msg)) {
											perror("send() failed");
										}	// if (send()) 

										memset(msg, '\0', MSG_SIZE);
										if (recv(server, msg, MSG_SIZE, 0) >= 0) {
											buffered_msg_num = atoi(strtok(msg, delim));
											// printf("client_num %d\n", client_num);
										}	// if (recv())

										memset(msg, '\0', MSG_SIZE);
										sprintf(msg, "%s", delim);
										if (send(server, msg, strlen(msg), 0) != strlen(msg)) {
											perror("send() failed");
										}	// if (send()) 

										for (i = 0; i < buffered_msg_num; i++) {
											memset(msg, '\0', MSG_SIZE);
											if (recv(server, msg, MSG_SIZE, 0) < 0) {
												perror("recv() failed");
											}	// if (recv()) 
											char* client_ip_addr = (char*)malloc(sizeof(char) * 16); 
											memset(client_ip_addr, '\0', 16); 
											strncpy(client_ip_addr, msg, 16);

											memset(msg, '\0', MSG_SIZE);
											sprintf(msg, "%s", delim);
											if (send(server, msg, strlen(msg), 0) != strlen(msg)) {
												perror("send() failed");
											}	// if (send()) 

											memset(msg, '\0', MSG_SIZE);
											if (recv(server, msg, MSG_SIZE, 0) < 0) {
												perror("recv() failed");
											}	// if (recv()) 

											cse4589_print_and_log("[RECEIVED:SUCCESS]\n");
											cse4589_print_and_log("msg from:%s\n[msg]:%s\n", client_ip_addr, msg);
											cse4589_print_and_log("[RECEIVED:END]\n");

											memset(msg, '\0', MSG_SIZE);
											sprintf(msg, "%s", delim);
											if (send(server, msg, strlen(msg), 0) != strlen(msg)) {
												perror("send() failed");
											}	// if (send()) 
											free(client_ip_addr); 
										}	// for (i) 
									}	// else 
								}	// if (validate_ip() && validate_port())

								if (LOGIN_FLAG) {
									cse4589_print_and_log("[LOGIN:SUCCESS]\n");
									cse4589_print_and_log("[LOGIN:END]\n"); 
								}	// if (LOGIN_FLAG)
								else {
									cse4589_print_and_log("[LOGIN:ERROR]\n");
									cse4589_print_and_log("[LOGIN:END]\n"); 
								}	// else 
								
							}	// else if (strcmp(cmd))
							else if (strcmp(cmd, "LOGOUT") == 0) {
								if (LOGIN_FLAG) {
									memset(msg, '\0', MSG_SIZE);
									sprintf(msg, "%s", "LOGOUT");
									if (send(server, msg, strlen(msg), 0) != strlen(msg)) {
										perror("send() failed");
									}	// if (send()) 

									memset(msg, '\0', MSG_SIZE);
									if (recv(server, msg, MSG_SIZE, 0) >= 0){
										if (strcmp(msg, delim) == 0) {
											LOGIN_FLAG = FALSE; 
											cse4589_print_and_log("[LOGOUT:SUCCESS]\n");
											cse4589_print_and_log("[LOGOUT:END]\n"); 
										}	// if (strcmp())
										else {
											cse4589_print_and_log("[LOGOUT:ERROR]\n");
											cse4589_print_and_log("[LOGOUT:END]\n"); 
										}	// else 
									}	// if (recv())

								}	// if (LOGIN_FLAG) 
							}	// else if (strcmp(cmd)) 
							else if (strcmp(cmd, "REFRESH") == 0) {
								if (LOGIN_FLAG) {
									memset(msg, '\0', MSG_SIZE);
									sprintf(msg, "%s", "REFRESH");
									if (send(server, msg, strlen(msg), 0) != strlen(msg)) {
										perror("send() failed");
									}	// if (send()) 

									delete_list(&list);

									memset(msg, '\0', MSG_SIZE);
									if (recv(server, msg, MSG_SIZE, 0) >= 0) {
										client_num = atoi(strtok(msg, delim));
									}	// if (recv())

									memset(msg, '\0', MSG_SIZE);
									sprintf(msg, "%s", delim);
									if (send(server, msg, strlen(msg), 0) != strlen(msg)) {
										perror("send() failed");
									}	// if (send()) 

									i = 0;
									memset(msg, '\0', MSG_SIZE);
									while (recv(server, msg, MSG_SIZE, 0) >= 0) {
										// https://stackoverflow.com/questions/15961253/c-correct-usage-of-strtok-r 
										token = strtok_r(msg, delim, &saveptr); 

										while (token) {
											char* client_hostname = strtok(token, " ");
											char* client_ip_addr = strtok(NULL, " ");
											int client_port = atoi(strtok(NULL, " "));
											int client_sock_index = atoi(strtok(NULL, " "));
											insert_node(&list, client_hostname, client_ip_addr, client_port, client_sock_index, FALSE);

											i++; 
											token = strtok_r(NULL, delim, &saveptr);
										}	// while (token) 

										fflush(stdout);
										memset(msg, '\0', MSG_SIZE);
										if (i >= client_num) {
											break;
										}	// if (i) 
									}	// while (recv())

								}	// if (LOGIN_FLAG) 
							}	// else if (strcmp(cmd)) 
							else if (strcmp(cmd, "SEND") == 0) {
								char* client_ip = (char*)malloc(16 * sizeof(char)); 
								memset(client_ip, '\0', 16); 
								strncpy(client_ip, strtok(NULL, " "), 16);
								if (validate_ip(client_ip) && (get_socket_index(&list, client_ip) != -1) && (strcmp(ip_addr, client_ip) != 0)) {
									char* msg_to_send_256 = get_msg_from_buffer(msg, MSG_SIZE, ' ', 2); 
									
									// send "SEND" to server 
									memset(msg, '\0', MSG_SIZE);
									sprintf(msg, "%s", "SEND");
									if (send(server, msg, strlen(msg), 0) != strlen(msg)) {
										perror("send() failed");
									}	// if (send()) 

									// if server sends acknowldgement, send client_ip 
									memset(msg, '\0', MSG_SIZE);
									if (recv(server, msg, MSG_SIZE, 0) >= 0){
										if (strcmp(msg, delim) != 0) {
											perror("Server did not send ACK");
										}	// if (strcmp())
										memset(msg, '\0', MSG_SIZE);
										sprintf(msg, "%s", client_ip);
										if (send(server, msg, strlen(msg), 0) != strlen(msg)) {
											perror("send() failed");
										}	// if (send()) 
									}	// if (recv())

									// if server sends acknowldgement, send the message 
									memset(msg, '\0', MSG_SIZE);
									if (recv(server, msg, MSG_SIZE, 0) >= 0){
										if (strcmp(msg, delim) != 0) {
											perror("Server did not send ACK");
										}	// if (strcmp())
										memset(msg, '\0', MSG_SIZE);
										sprintf(msg, "%s", msg_to_send_256);
										if (send(server, msg, strlen(msg), 0) != strlen(msg)) {
											perror("send() failed");
										}	// if (send()) 
									}	// if (recv())

									// The blocked sender, however, 
									// will be unaware of this blocking and should execute the SEND command without any error. 
									memset(msg, '\0', MSG_SIZE);
									if (recv(server, msg, MSG_SIZE, 0) >= 0){
										if (strcmp(msg, delim) == 0) {
											cse4589_print_and_log("[SEND:SUCCESS]\n");
											cse4589_print_and_log("[SEND:END]\n"); 
										}	// if (strcmp())
										else {
											cse4589_print_and_log("[SEND:ERROR]\n");
											cse4589_print_and_log("[SEND:END]\n"); 
										}	// else 
									}	// if (recv())
									free(msg_to_send_256); 
								}	// if (validate_ip())
								else {
									cse4589_print_and_log("[SEND:ERROR]\n");
									cse4589_print_and_log("[SEND:END]\n"); 
								}	// else 
								free(client_ip); 
							}	// else if (strcmp(cmd))
							else if(strcmp(cmd, "BROADCAST") ==0) {
								//send message to all logged-in clients
								if (LOGIN_FLAG) {
									char* msg_to_send_256 = get_msg_from_buffer(msg, MSG_SIZE, ' ', 1); 
									// send "BROADCAST" to server 
									memset(msg, '\0', MSG_SIZE);
									sprintf(msg, "%s", "BROADCAST");
									if (send(server, msg, strlen(msg), 0) != strlen(msg)) {
										perror("send() failed");
									}	// if (send()) 

									// if server sends acknowldgement, send the message 
									memset(msg, '\0', MSG_SIZE);
									if (recv(server, msg, MSG_SIZE, 0) >= 0){
										if (strcmp(msg, delim) != 0) {
											perror("Server did not send ACK");
										}	// if (strcmp())
										memset(msg, '\0', MSG_SIZE);
										sprintf(msg, "%s", msg_to_send_256);
										if (send(server, msg, strlen(msg), 0) != strlen(msg)) {
											perror("send() failed");
										}	// if (send()) 
									}	// if (recv())

									// The blocked sender, however, 
									// will be unaware of this blocking and should execute the SEND command without any error. 
									memset(msg, '\0', MSG_SIZE);
									if (recv(server, msg, MSG_SIZE, 0) >= 0){
										if (strcmp(msg, delim) == 0) {
											cse4589_print_and_log("[BROADCAST:SUCCESS]\n");
											cse4589_print_and_log("[BROADCAST:END]\n"); 
										}	// if (strcmp())
										else {
											cse4589_print_and_log("[BROADCAST:ERROR]\n");
											cse4589_print_and_log("[BROADCAST:END]\n"); 
										}	// else 
									}	// if (recv())
									free(msg_to_send_256); 
								}	// if (LOGIN_FLAG)
							}	// else if (strcmp(cmd))
							else if(strcmp(cmd, "BLOCK")== 0){
								if (LOGIN_FLAG) {
									char* client_ip = (char*)malloc(16 * sizeof(char)); 
									memset(client_ip, '\0', 16); 
									strncpy(client_ip, strtok(NULL, " "), 16);
									if (validate_ip(client_ip) && (get_socket_index(&list, client_ip) != -1)) {
										memset(msg, '\0', MSG_SIZE);
										sprintf(msg, "%s", "BLOCK");
										if (send(server, msg, strlen(msg), 0) != strlen(msg)) {
											perror("send() failed");
										}	// if (send()) 

										// if server sends acknowldgement, send client_ip 
										memset(msg, '\0', MSG_SIZE);
										if (recv(server, msg, MSG_SIZE, 0) >= 0){
											if (strcmp(msg, delim) != 0) {
												perror("Server did not send ACK");
											}	// if (strcmp())
											memset(msg, '\0', MSG_SIZE);
											sprintf(msg, "%s", client_ip);
											if (send(server, msg, strlen(msg), 0) != strlen(msg)) {
												perror("send() failed");
											}	// if (send()) 
										}	// if (recv())

										memset(msg, '\0', MSG_SIZE);
										if (recv(server, msg, MSG_SIZE, 0) >= 0){
											if (strcmp(msg, delim) == 0) {
												cse4589_print_and_log("[BLOCK:SUCCESS]\n");
												cse4589_print_and_log("[BLOCK:END]\n"); 
											}	// if (strcmp())
											else {
												cse4589_print_and_log("[BLOCK:ERROR]\n");
												cse4589_print_and_log("[BLOCK:END]\n"); 
											}	// else 
										}	// if (recv()) 
									}	// if (validate_ip())
									else {
										cse4589_print_and_log("[BLOCK:ERROR]\n");
										cse4589_print_and_log("[BLOCK:END]\n"); 
									}	// else 
									free(client_ip); 
								}	// if (LOGIN_FLAG) 
							}	// else if (strcmp(cmd)) 
							else if(strcmp(cmd, "UNBLOCK")== 0){
								if (LOGIN_FLAG) {
									char* client_ip = (char*)malloc(16 * sizeof(char)); 
									memset(client_ip, '\0', 16); 
									strncpy(client_ip, strtok(NULL, " "), 16);
									if (validate_ip(client_ip) && (get_socket_index(&list, client_ip) != -1)) {
										memset(msg, '\0', MSG_SIZE);
										sprintf(msg, "%s", "UNBLOCK");
										if (send(server, msg, strlen(msg), 0) != strlen(msg)) {
											perror("send() failed");
										}	// if (send()) 

										// if server sends acknowldgement, send client_ip 
										memset(msg, '\0', MSG_SIZE);
										if (recv(server, msg, MSG_SIZE, 0) >= 0){
											if (strcmp(msg, delim) != 0) {
												perror("Server did not send ACK");
											}	// if (strcmp())
											memset(msg, '\0', MSG_SIZE);
											sprintf(msg, "%s", client_ip);
											if (send(server, msg, strlen(msg), 0) != strlen(msg)) {
												perror("send() failed");
											}	// if (send()) 
										}	// if (recv())

										memset(msg, '\0', MSG_SIZE);
										if (recv(server, msg, MSG_SIZE, 0) >= 0){
											if (strcmp(msg, delim) == 0) {
												cse4589_print_and_log("[UNBLOCK:SUCCESS]\n");
												cse4589_print_and_log("[UNBLOCK:END]\n"); 
											}	// if (strcmp())
											else {
												cse4589_print_and_log("[UNBLOCK:ERROR]\n");
												cse4589_print_and_log("[UNBLOCK:END]\n"); 
											}	// else 
										}	// if (recv()) 
									}	// if (validate_ip())
									else {
										cse4589_print_and_log("[UNBLOCK:ERROR]\n");
										cse4589_print_and_log("[UNBLOCK:END]\n"); 
									}	// else 
									free(client_ip); 
								}	// if (LOGIN_FLAG) 
							}	// else if (strcmp(cmd)) 
							else if (strcmp(cmd, "EXIT") == 0) {
								if (LOGIN_FLAG) {
									LOGIN_FLAG = FALSE;
								}	// if (LOGIN_FLAG) 
								exit(0);
							}	// else if (strcmp(cmd)) 
							else {
								perror("Not supported!\n");
							}	// else 
							
							free(msg);
							free(cmd); 
						}	// if (selret) 
						else if (sock_index == server) {
							if (LOGIN_FLAG) {
								/* Initialize buffer to receieve response */
								char *buffer = (char*)malloc(sizeof(char)*BUFFER_SIZE);
								memset(buffer, '\0', BUFFER_SIZE);
								if (recv(sock_index, buffer, BUFFER_SIZE, 0) < 0){
									perror("recv() failed");
								}	// if (recv())

								if (strcmp(buffer, "SEND") == 0) {
									memset(buffer, '\0', BUFFER_SIZE);
									sprintf(buffer, "%s", delim);
									if (send(sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
										perror("send() failed");
									}	// if (send()) 

									memset(buffer, '\0', BUFFER_SIZE);
									if (recv(sock_index, buffer, BUFFER_SIZE, 0) < 0){
										perror("recv() failed");
									}	// if (recv())
									
									char* client_ip = (char*)malloc(16 * sizeof(char)); 
									memset(client_ip, '\0', 16); 
									strncpy(client_ip, buffer, 16);
									
									memset(buffer, '\0', BUFFER_SIZE);
									sprintf(buffer, "%s", delim);
									if (send(sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
										perror("send() failed");
									}	// if (send()) 

									memset(buffer, '\0', BUFFER_SIZE);
									if (recv(sock_index, buffer, BUFFER_SIZE, 0) < 0){
										perror("recv() failed");
									}	// if (recv())
									
									char* received_msg = (char*)malloc(256 * sizeof(char)); 
									memset(received_msg, '\0', 256); 
									memcpy(received_msg, buffer, 256);

									cse4589_print_and_log("[RECEIVED:SUCCESS]\n");
									cse4589_print_and_log("msg from:%s\n[msg]:%s\n", client_ip, received_msg);
									cse4589_print_and_log("[RECEIVED:END]\n");

									memset(buffer, '\0', BUFFER_SIZE);
									sprintf(buffer, "%s", delim);
									if (send(sock_index, buffer, strlen(buffer), 0) != strlen(buffer)) {
										perror("send() failed");
									}	// if (send()) 
									free(client_ip); 
									free(received_msg); 
								} 	// if (strcmp(buffer)) 
								free(buffer); 
							}	// if (LOGIN_FLAG) 
						}	// else if (sock_index) 
						else {

						}	// else 
					}	// if(FD_ISSET()) 
				}	// for (sock_index) 
			}	// if (selret) 
		}	// while (TRUE) 
			

	}	// else if (argv[1][0]) 
	else {
		perror("The first argument is neither 's' nor 'c'.\n"); 
		exit(-1);
	}	// else 

	return 0;
}
				