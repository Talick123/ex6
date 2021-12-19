/*
- via argument vector receives
    - IP address of prime server
    - Port number for prime server
    - IP address for palindrome server
    - Port number for palindrome server

in a loop:
    - reads from user p and then integer (non-negative) OR
     q and then string
    - sends request to proper server, receives answer, prints on screen
    - ends when reads 'e'

inferno-03 = 10.3.10.25
*/


#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <signal.h>


#define ARGC_SIZE 5 //1: prime server ip
                    //2: prime server port
                    //3: palindrome server ip
                    //4: palindrome server port

//---------const section ---------------------------

const int LEN_STR_MAX = 100;

// --------prototype section------------------------

void check_argc(int argc);
int connect_to_socket(char ip[], char port[], struct addrinfo con_kind, struct addrinfo *addr_info_res);
void get_user_req(int my_socket_p, int my_socket_q);
void perrorandexit(char *msg);
void p_request(int my_socket_p);
void q_request(int my_socket_q);

// --------main section------------------------

int main(int argc, char *argv[])
{
    int my_socket_p, my_socket_q;
    struct addrinfo con_kind, *addr_info_res = NULL;

    check_argc(argc);
    srand(atoi(argv[1]));
    srand(atoi(argv[3]));

    my_socket_p = connect_to_socket(argv[1], argv[2], con_kind, addr_info_res);
    my_socket_q = connect_to_socket(argv[3], argv[4], con_kind, addr_info_res);

    get_user_req(my_socket_p, my_socket_q);
    freeaddrinfo(addr_info_res);

    return EXIT_SUCCESS;
}
//-------------------------------------------------

void get_user_req(int my_socket_p, int my_socket_q)
{
    char c;

    while(true)
    {
        c = getchar();
        switch (c)
        {
            //read num to check is prime
            case 'p':
                p_request(my_socket_p);
                break;
            //read series of num to check is palindrome
            case 'q':
                q_request(my_socket_q);
                break;
            //end process
            case 'e':
                return;
            default:
                break;
        }
    }
}

//------------------------------------------------
// get int and sends to prime server - print the result
void p_request(int my_socket_p)
{
    int num, res;
    int rc;

    scanf(" %d", &num);
    getchar();

    write(my_socket_p ,&num ,sizeof(int));
    rc = read(my_socket_p ,&res ,sizeof(int));
    if(rc > 0)
    {
        printf("is prime ? %s\n", res ? "yes" : "no");
    }
    else
    {
        perrorandexit("read() failed");
    }
}

//------------------------------------------------
// get string and sends to palindrome server - print the result
void q_request(int my_socket_q)
{
	int rc, res;
    char str[LEN_STR_MAX];

	scanf("%s", str);
    write(my_socket_q ,&str ,sizeof(char)*LEN_STR_MAX);
    rc = read(my_socket_q ,&res ,sizeof(int));
    if(rc > 0)
    {
        printf("is palindrome ? %s\n", res ? "yes" : "no");
    }
    else
    {
        perrorandexit("read() failed");
    }
}

//-------------------------------------------------

void check_argc(int argc)
{
	if(argc != ARGC_SIZE)
		perrorandexit("Error! Incorrect number of arguments\n");
}

//-------------------------------------------------

int connect_to_socket(char ip[], char port[], struct addrinfo con_kind, struct addrinfo *addr_info_res)
{
    int my_socket, rc;

    memset(&con_kind, 0, sizeof(con_kind));
    con_kind.ai_family = AF_UNSPEC;
    con_kind.ai_socktype = SOCK_STREAM;

    if((rc = getaddrinfo(ip, port, &con_kind, &addr_info_res)!= 0))
        perrorandexit("getaddrinfo failed\n");

    my_socket = socket(addr_info_res->ai_family, addr_info_res->ai_socktype, addr_info_res->ai_protocol); //returns file descriptor or -1 if failed

    if(my_socket < 0)
        perrorandexit("socket failed\n");

    rc = connect(my_socket, addr_info_res->ai_addr, addr_info_res->ai_addrlen);

    if(rc) //0 if succeeded, -1 if not
        perrorandexit("connect failed\n");

    return my_socket;
}

//-------------------------------------------------

void perrorandexit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
