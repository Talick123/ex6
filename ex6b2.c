/*


File: ex6b1.c ex6b2.c ex6b3.c
Interactions Between Prime Checker, Palindrome Checker and Client
=====================================================================
Written by: Tali Kalev, ID:208629691, Login: talikal
		and	Noga Levy, ID:315260927, Login: levyno

ex6b1: reads number via socket and sends back whether number is prime or not.
ex6b2: reads string via socket and sends back whether string is palindrome or not
ex6b3: connects to both sockets. reads from user p or q, and then
number of series of numbers respectively. Enters into the appropriate socket
the data and waits for response. Prints result onto screen.

Compile: gcc -Wall ex6b1.c -o ex6b1
         gcc -Wall ex6b2.c -o ex6b2
        gcc -Wall ex6b3.c -o ex6b3
     (ex6b1 = prime checker, ex6b2 = palindrome checker, ex6b3 = client)

Run: run all three programs one after the other. First 2 programs receive respective
    ports, third program is run with IP and port of prime server and then IP and
    port of palindrome server.
    Example:
        ./ex6b1 12121
        ./ex6b2 13131
        ./ex6b3 10.3.10.25 12121 10.3.10.25 13131

Input: Only ex6b3 receives input. If input is p and then number, ex6b1 checks if
        it is prime. If input is q and then a series of numbers ending in 0, ex6b2
        checks if it is a palindrome.
        Example: p
                 7

Output: ex6c3 prints answer of other programs appropriately
        Example:
        Is Prime

*/

// Palindrome Server

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

#define ARGC_SIZE 2 //port
#define NUM_OF_CLIENTS 3

// --------const and enum section------------------------

const int LEN_STR_MAX = 100;

// --------global variables section------------------------

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
int is_pal(char str[], int n);
void catch_sigint(int signum);

// --------main section------------------------

int main(int argc, char *argv[])
{
    struct addrinfo con_kind;

    signal(SIGINT, catch_sigint);
    check_argc(argc);
    main_socket = init_socket(argv[1], con_kind, addr_info_res);

    get_requests(main_socket);

    return EXIT_SUCCESS;
}

//-------------------------------------------------

void catch_sigint(int signum)
{
    close(main_socket);
    freeaddrinfo(addr_info_res);
    exit(EXIT_SUCCESS);
}

//-------------------------------------------------

void get_requests(int main_socket)
{
    int rc, fd, res;
    char str[LEN_STR_MAX];
	fd_set rfd ,c_rfd;

	rc = listen(main_socket,NUM_OF_CLIENTS);
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

		for(fd = main_socket + 1; fd < getdtablesize();fd++)
		{
			if(FD_ISSET(fd ,&c_rfd))
			{
				rc = read(fd ,&str ,sizeof(char)*LEN_STR_MAX);
				if(rc > 0)
				{
					res = is_pal(str, strlen(str));
					write(fd ,&res ,sizeof(int));
				}
				else if(rc == 0)
                {
                    close(fd);
                    FD_CLR(fd, &rfd);
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

int is_pal(char str[], int n)
{
    // Initialise flag to zero.
    int flag = 1;

    // Loop till array size n/2.
    for (int i = 0; i <= n / 2 && n != 0; i++)
    {
        // Check if first and last element are different
        // Then set flag to 1.
        if (str[i] != str[n - 1- i ]) {
            flag = 0;
            break;
        }
    }

    // If flag is set then print Not Palindrome
    // else print Palindrome.
    return flag;
}

//-------------------------------------------------

void perrorandexit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
