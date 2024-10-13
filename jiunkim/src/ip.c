#include "../include/global.h" 
#include "../include/logger.h" 
#include "../include/ip.h" 

#include <stdio.h>

int get_ip(char* ip_addr)
{
    // https://ubmnc.wordpress.com/2010/09/22/on-getting-the-ip-name-of-a-machine-for-chatty/ 
    // from pa1_demo_code/client.c 
    int dummy_fdsocket;
    struct addrinfo dummy_hints, *dummy_res;
    char* google_public_dns_server_ip = "8.8.8.8";
    char* google_public_dns_server_port = "53";
    // https://stackoverflow.com/questions/20941027/getsockname-c-not-setting-value 
    // https://support.sas.com/documentation/onlinedoc/ccompiler/doc750/html/lr2/zockname.htm 
    struct sockaddr_in dummy_sock_addr;
    int dummy_sock_len = sizeof(dummy_sock_addr);

    // 1. Create a UDP socket 
    /* Set up dummy_hints structure */	
    memset(&dummy_hints, 0, sizeof(dummy_hints));
    dummy_hints.ai_family = AF_INET;
    dummy_hints.ai_socktype = SOCK_DGRAM;	// for UDP 

    /* Fill up address structures */	
    if (getaddrinfo(google_public_dns_server_ip, google_public_dns_server_port, &dummy_hints, &dummy_res) != 0) {
        return FALSE;
    }	// if (getaddrinfo())

    /* Socket */
    dummy_fdsocket = socket(dummy_res->ai_family, dummy_res->ai_socktype, dummy_res->ai_protocol);
    if(dummy_fdsocket < 0) {
        return FALSE;
    }	// if (dummy_fdsocket) 

    // 2. connect the socket to 8.8.8.8:53 (Google's public DNS server) 
    /* Connect */
    if(connect(dummy_fdsocket, dummy_res->ai_addr, dummy_res->ai_addrlen) < 0) {
        return FALSE;
    }	// if (connect()) 

    // 3. Call getsockname() on the dummy UDP socket 
    // https://support.sas.com/documentation/onlinedoc/ccompiler/doc750/html/lr2/zockname.htm 
    if (getsockname(dummy_fdsocket, (struct sockaddr*)&dummy_sock_addr, &dummy_sock_len) == -1) {
        return FALSE;
    }	// if (getsockname())

    // 4. Close the dummy UDP socket 
    freeaddrinfo(dummy_res);
    // https://www.gnu.org/software/libc/manual/html_node/Closing-a-Socket.html#:~:text=2%20Closing%20a%20Socket,tries%20to%20complete%20this%20transmission. 
    close(dummy_fdsocket);

    // https://www.tutorialspoint.com/c_standard_library/c_function_sprintf.htm 
    sprintf(ip_addr, "%s", inet_ntoa(dummy_sock_addr.sin_addr));

    return TRUE;
}