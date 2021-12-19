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
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define ARGC_SIZE 4

// --------const and enum section------------------------

const int START = 1;
const int END = -1;

// --------prototype section------------------------

void check_argc(int argc);
int connect_to_socket(char ip[], char port[], struct addrinfo con_kind,
					   struct addrinfo *addr_info_res);
void generate_numbers(int my_socket);
bool prime(int num);
void print_data(int max_count,int max_prime ,int new_primes);
void perrorandexit(char *msg);

// --------main section------------------------

int main(int argc, char *argv[])
{
    int my_socket;
    struct addrinfo con_kind, *addr_info_res = NULL;

    check_argc(argc);
    srand(atoi(argv[1]));
    my_socket = connect_to_socket(argv[2], argv[3], con_kind,
									addr_info_res);
    generate_numbers(my_socket);
    close(my_socket);
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

int connect_to_socket(char ip[], char port[], struct addrinfo con_kind,
                      struct addrinfo *addr_info_res)
{
    int my_socket, rc;

    memset(&con_kind, 0, sizeof(con_kind));
    con_kind.ai_family = AF_UNSPEC;
    con_kind.ai_socktype = SOCK_STREAM;

    if((rc = getaddrinfo(ip, port, &con_kind, &addr_info_res)!= 0))
        perrorandexit("getaddrinfo failed\n");

    my_socket = socket(addr_info_res->ai_family,
                       addr_info_res->ai_socktype,
                       addr_info_res->ai_protocol);

    if(my_socket < 0)
        perrorandexit("socket failed\n");

    rc = connect(my_socket, addr_info_res->ai_addr, addr_info_res->ai_addrlen);

    if(rc)
        perrorandexit("connect failed\n");

    return my_socket;
}

//-------------------------------------------------

void generate_numbers(int my_socket)
{
    int rc;
    int max_count, max_prime, num, new_primes, answer;
    max_count = new_primes = 0;

    //writing to server that its ready to start
    write(my_socket, &START , sizeof(int));
    rc = read(my_socket, &answer, sizeof(int));

    while(answer != END)
    {
        num = (rand() + 2);

        if(prime(num))
        {
            write(my_socket, &num, sizeof(int));
            rc = read(my_socket, &answer, sizeof(int));
            if(rc > 0)
            {
                if(answer > max_count || max_count == 0)
                {
					if(answer == 0)
						max_count = 1;
					else
						max_count = answer;

                    max_prime = num;
                }
                if(answer == 0)
                    new_primes++;
            }
            else
                perrorandexit("read failed\n");
        }
    }
    print_data(max_count, max_prime, new_primes);
}

//-------------------------------------------------

//check is prime
bool prime(int num)
{
	int i;
	for(i = 2; i*i <= num; i++)
		if(num % i == 0)
			return false;

	return true;
}

//-------------------------------------------------

void print_data(int max_count,int max_prime ,int new_primes)
{
    printf("Process %d sent %d different new primes.\n",
        (int)getpid(), new_primes);

     printf("The prime it sent most was %d, %d times. \n",
        	   max_prime, max_count);
}

//-------------------------------------------------

void perrorandexit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
