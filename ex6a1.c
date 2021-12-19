/*

File: ex6a1.c ex6a2.c
Generate and Collect Primes from Socket
=====================================================================
Written by: Tali Kalev, ID:208629691, Login: talikal
		and	Noga Levy, ID:315260927, Login: levyno

This program runs with 4 different processes. Three processes that generates
random numbers, when the number is prime the process sends it to main process via socket.
When the main process sees that the array is full, it sends message to other
processes, prints out data and ends. The main proccess prints how many different
numbers it received, the smallest number and the biggest number. The other processes
prints how many new primes it sent and the prime it sent most.

Compile: gcc -Wall ex6a1.c -o ex6a1
         gcc -Wall ex6a2.c -o ex6a2
     (ex6a1 = main process, ex6a2 = sub process)

Run: for start run the main process with port number.
    Then, run 3 times the sub processes and send to the vector
    arguments the number of process (1-3), IP address of main process and port
    number:
        ./ex6a1 12121
        ./ex6a2 1 10.3.10.25 12121
        ./ex6a2 2 10.3.10.25 12121
        ./ex6a2 3 10.3.10.25 12121

Input: No Input

Output:
    From main process (ex6a1) = minimum prime, max prime and number of
    different numbers in the array.
    Example: The number of different primes received is: 1000.
             The max prime is: 2146710941.
             The min prime is: 2701151.

    From sub process (ex6a2) = prime number they send the most to main process
    Example: Process 683348 sent 326 different new primes.
             The prime it sent most was 747983063, 1 times.

*/
// --------include section------------------------

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>

#define ARR_SIZE 1000
#define ARGC_SIZE 2
#define NUM_OF_CLIENTS 3

// --------const and enum section------------------------

const int START = 1;
const int END = -1;

// --------prototype section------------------------

void check_argc(int argc);
int init_socket(char port[], struct addrinfo con_kind,
				struct addrinfo *addr_info_res);
void fill_arr(int main_socket);
int count_appearances(int arr[], int filled, int num);
void print_data(int new_prime, int max, int min);
void perrorandexit(char *msg);

// --------main section------------------------

int main(int argc, char *argv[])
{
    int main_socket;
    struct addrinfo con_kind, *addr_info_res = NULL;

    check_argc(argc);
    main_socket = init_socket(argv[1], con_kind, addr_info_res);
    fill_arr(main_socket);
    close(main_socket);
    freeaddrinfo(addr_info_res);

    return EXIT_SUCCESS;
}

//-------------------------------------------------

void check_argc(int argc)
{
	if(argc != ARGC_SIZE)
		perrorandexit("Error! Incorrect number of arguments\n");
}

//-------------------------------------------------

int init_socket(char port[], struct addrinfo con_kind,
				struct addrinfo *addr_info_res)
{
    int main_socket, rc;

    memset(&con_kind, 0, sizeof(con_kind));
    con_kind.ai_family = AF_UNSPEC;
    con_kind.ai_socktype = SOCK_STREAM;
    con_kind.ai_flags = AI_PASSIVE;

    if((rc = getaddrinfo(NULL, port, &con_kind, &addr_info_res)!= 0))
        perrorandexit("getaddrinfo failed\n");

    main_socket = socket(addr_info_res->ai_family,
						 addr_info_res->ai_socktype,
						 addr_info_res->ai_protocol);

    if(main_socket < 0)
        perrorandexit("socket failed\n");

    rc = bind(main_socket, addr_info_res->ai_addr, addr_info_res->ai_addrlen);

    if(rc)
        perrorandexit("bind failed\n");

    return main_socket;
}

//-------------------------------------------------

void fill_arr(int main_socket)
{
    int rc, serving_socket, filled, num, new_prime, min, max, count;
    int arr[ARR_SIZE];
    fd_set rfd, c_rfd;
    int fd, client_count, answer;

    filled = new_prime = min = max = client_count = 0;

    rc = listen(main_socket, NUM_OF_CLIENTS);
    if(rc)
        perrorandexit("listen failed\n");

    FD_ZERO(&rfd);
    FD_SET(main_socket, &rfd);

    //receiving ones from all clients
    while(client_count !=  NUM_OF_CLIENTS)
    {
        c_rfd = rfd;

        rc = select(getdtablesize(), &c_rfd, NULL, NULL, NULL); //check rc?

        if(FD_ISSET(main_socket, &c_rfd))
        {
            serving_socket = accept(main_socket, NULL, NULL);
            if(serving_socket >= 0)
                FD_SET(serving_socket, &rfd);
        }
        for(fd = main_socket + 1; fd < getdtablesize(); fd++)
        {
            if(FD_ISSET(fd, &c_rfd))
            {
                rc = read(fd, &answer, sizeof(int));
                if(rc > 0)
                    client_count++;
            }
            if(client_count == NUM_OF_CLIENTS)
                break;
        }
    }

    //sending to clients to start
    for(fd = main_socket + 1; fd < getdtablesize(); fd++)
    {
        write(fd, &START, sizeof(int));
	}

    //filling array
    while(filled < ARR_SIZE)
    {
        c_rfd = rfd;

        rc = select(getdtablesize(), &c_rfd, NULL, NULL, NULL);

        if(FD_ISSET(main_socket, &c_rfd))
        {
            serving_socket = accept(main_socket, NULL, NULL);
            if(serving_socket >= 0)
                FD_SET(serving_socket, &rfd);
        }

        for(fd = main_socket + 1; fd < getdtablesize(); fd++)
        {
            if(FD_ISSET(fd, &c_rfd))
            {
                rc = read(fd, &num, sizeof(int));
                if(rc > 0)
                {
                    if(filled >= ARR_SIZE)
                    {
                        write(fd, &END, sizeof(int));
                        close(fd);
                        FD_CLR(fd, &rfd);
                    }
                    else
                    {
                        count = count_appearances(arr, filled, num);
                        write(fd, &count, sizeof(int));

                        if(count == 0)
                            new_prime++;
                        if(num > max)
                            max = num;
                        if(num < min || min == 0)
                            min = num;

                        arr[filled] = num;
                        filled++;
                    }
                }
                else if(rc == 0)
                {
                    close(fd);
                    FD_CLR(fd, &rfd);
                }
                else
                    perrorandexit("read failed\n");
            }
        }
    }

    //closing any remaing waiting clients
    for(fd = main_socket + 1; fd < getdtablesize(); fd++)
    {
        if(FD_ISSET(fd, &c_rfd))
        {
            rc = read(fd, &num, sizeof(int));
            if(rc > 0)
            {
                write(fd, &END, sizeof(int));
                close(fd);
                FD_CLR(fd, &rfd);
            }
		}
    }
    print_data(new_prime, max, min);
}

//-------------------------------------------------

int count_appearances(int arr[], int filled, int num)
{
    int counter = 0, index;

    for(index = 0; index < filled; index++)
        if(arr[index] == num)
            counter++;

    return counter;
}

//-------------------------------------------------

void print_data(int new_prime, int max, int min)
{
	printf("The number of different primes received is: %d.\n", new_prime);
	printf("The max prime is: %d.\n The min prime is: %d.\n", max, min);
}

//-------------------------------------------------

void perrorandexit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
