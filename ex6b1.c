/*
- via argument vector receives port number

- opens socket (for a number of clients)
- in a loop
    - reads non-negative integer from client
    - returns to client if the number is prime or not
    - ends with SIGINT, in signal handler, releases socket

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

#define ARGC_SIZE 2 // port
#define NUM_OF_CLIENTS 3

// --------const and enum section------------------------

//enum Status {START = 1, END = -1};
const int START = 1;
const int END = -1;

int main_socket;
struct addrinfo *addr_info_res = NULL;
// --------prototype section------------------------

void check_argc(int argc);
int init_socket(char port[], struct addrinfo con_kind, struct addrinfo *addr_info_res);
void get_requests(int main_socket);
void wait_for_clients(fd_set rfd, int main_socket);
int count_appearances(int arr[], int filled, int num);
void print_data(int new_prime, int max, int min);
void perrorandexit(char *msg);
int is_prime(int num);
void catch_sigint(int signum);
void perrorandexit(char *msg);

// --------main section------------------------

int main(int argc, char *argv[])
{
    //int main_socket;
    struct addrinfo con_kind;//, *addr_info_res = NULL;
    signal(SIGINT, catch_sigint);


    check_argc(argc);
    main_socket = init_socket(argv[1], con_kind, addr_info_res);

    get_requests(main_socket);
    close(main_socket);
    freeaddrinfo(addr_info_res);

    return EXIT_SUCCESS;
}

//-------------------------------------------------

void catch_sigint(int signum)
{
    close(main_socket);
    freeaddrinfo(addr_info_res);
}

//-------------------------------------------------

void get_requests(int main_socket)
{
	int num, rc, fd;
    int res;
	fd_set rfd ,c_rfd;

	rc = listen(main_socket, NUM_OF_CLIENTS);
	if(rc)
	{
        perrorandexit("listen failed");
	}

	FD_ZERO(&rfd);
	FD_SET(main_socket ,&rfd);

	while(1)
	{
		c_rfd = rfd;
		rc = select(getdtablesize() ,&c_rfd ,NULL ,NULL ,NULL);

		if(FD_ISSET(main_socket ,&c_rfd))
		{
			main_socket = accept(main_socket ,NULL ,NULL);
			if(main_socket >= 0)
			{
				FD_SET(main_socket ,&rfd);
			}
		}

		for(fd = main_socket + 1; fd < getdtablesize(); fd++)
		{
			if(FD_ISSET(fd ,&c_rfd))
			{
				rc = read(fd ,&num ,sizeof(int));
				if(rc > 0)
				{
					res = is_prime(num);
					write(fd ,&res ,sizeof(int));
				}
				else
				{
					perrorandexit("read() failed");
				}
			}
		}
	}
}

//-------------------------------------------------

void check_argc(int argc)
{
	if(argc != ARGC_SIZE)
		perrorandexit("Error! Incorrect number of arguments\n");
}


//-------------------------------------------------

int init_socket(char port[], struct addrinfo con_kind, struct addrinfo *addr_info_res)
{
    int main_socket, rc;

    printf("Port is: %s\n", port);

    memset(&con_kind, 0, sizeof(con_kind));
    con_kind.ai_family = AF_UNSPEC;
    con_kind.ai_socktype = SOCK_STREAM;
    con_kind.ai_flags = AI_PASSIVE;

    if((rc = getaddrinfo(NULL, port, &con_kind, &addr_info_res)!= 0))
        perrorandexit("getaddrinfo failed\n");

    main_socket = socket(addr_info_res->ai_family, addr_info_res->ai_socktype, addr_info_res->ai_protocol);

    if(main_socket < 0)
        perrorandexit("socket failed\n");

    rc = bind(main_socket, addr_info_res->ai_addr, addr_info_res->ai_addrlen);

    if(rc)
        perrorandexit("bind failed\n");

    return main_socket;
}

//-------------------------------------------------

int is_prime(int num)
{
    int i;
    for(i = 2; i*i <= num; i++)
    {
        if(num % i == 0)
        	return 0;
    }
    return 1;
}

//-------------------------------------------------

void perrorandexit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
