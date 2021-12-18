/*
- receives port via argument vector
- waits for 3 messages via socket, sends back 3 to tell clients to start

- defines array of integer ARR_SIZE = 1000
- from each client, reads number, adds to array
- sends feedback to client that sent the number
- when finished to fill array, sends to clients -1
Output:
    From main process (ex6a1) = minimum prime, max prime and number of
    different numbers in the array.
    Example: The number of different primes received is: 168
             The max prime is: 997. The min primes is: 2
    From sub process (ex6a2) = prime number they send the most to main process
    Example: Process 1101373 sent the prime 233, 14 times

inferno-03 = 10.3.10.25
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

//enum Status {START = 1, END = -1};
const int START = 1;
const int END = -1;

// --------prototype section------------------------

void check_argc(int argc);
int init_socket(char port[], struct addrinfo con_kind, struct addrinfo *addr_info_res);
void fill_arr(int main_socket);
void wait_for_clients(fd_set rfd, int main_socket);
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

void fill_arr(int main_socket)
{
	printf("in fill arr\n");

    int rc, serving_socket, filled = 0, num, new_prime = 0, min = 0, max = 0, count;
    int arr[ARR_SIZE];
    fd_set rfd, c_rfd;
    int fd;

    int client_count = 0, answer;

    rc = listen(main_socket, NUM_OF_CLIENTS); //T: 3?
    if(rc)
        perrorandexit("listen failed\n");

    FD_ZERO(&rfd);
    FD_SET(main_socket, &rfd);

    wait_for_clients(rfd, main_socket);


    //receiving ones from all clients
    while(client_count !=  NUM_OF_CLIENTS)
    {
		printf("client count is: %d\n", client_count);
        c_rfd = rfd;
        rc = select(getdtablesize(), &c_rfd, NULL, NULL, NULL); //check rc?
        printf("rc is: %d\n", rc);
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

	printf("after wait\n");
    //filling array
    while(filled < ARR_SIZE)
    {
		printf("filled is: %d\n", filled);
		printf("in main while\n");
        c_rfd = rfd;

		printf("before select\n");//gets to here
        rc = select(getdtablesize(), &c_rfd, NULL, NULL, NULL); //check rc?
        printf("rc is: %d\n", rc);

		printf("after select\n");
        if(FD_ISSET(main_socket, &c_rfd))
        {
            serving_socket = accept(main_socket, NULL, NULL);
            printf("after accept\n");
            if(serving_socket >= 0)
                FD_SET(serving_socket, &rfd);
        }

        for(fd = main_socket + 1; fd < getdtablesize(); fd++)
        {
            if(FD_ISSET(fd, &c_rfd))
            {
				printf("reading\n");
                rc = read(fd, &num, sizeof(int));
                if(rc > 0)
                {
                    if(filled >= ARR_SIZE)
                    {
						printf("writing\n");
                        write(fd, &END, sizeof(int));
                        close(fd);
                        FD_CLR(fd, &rfd);
                    }
                    else
                    {
                        count = count_appearances(arr, filled, num);
                        printf("writing\n");
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

void wait_for_clients(fd_set rfd, int main_socket)
{

	printf("in wait_for_clients\n");
	/*
    int rc, client_count = 0, serving_socket, fd, answer;
    fd_set c_rfd;

    //receiving ones from all clients
    while(client_count !=  NUM_OF_CLIENTS)
    {
		printf("client count is: %d\n", client_count);
        c_rfd = rfd;
        rc = select(getdtablesize(), &c_rfd, NULL, NULL, NULL); //check rc?
        printf("rc is: %d\n", rc);
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
        write(fd, &START, sizeof(int));
*/
}

//-------------------------------------------------

int count_appearances(int arr[], int filled, int num)
{
	printf("in count appearances\n");
    int counter = 0, index;

    for(index = 0; index < filled; index++)
        if(arr[index] == num)
            counter++;

    return counter;
}

//-------------------------------------------------

void print_data(int new_prime, int max, int min)
{
	printf("The number of different primes received is: %d\n", new_prime);
	printf("The max prime is: %d\n. The min prime is: %d\n", max, min);
}


//-------------------------------------------------

void perrorandexit(char *msg)
{
    perror(msg);
    exit(EXIT_FAILURE);
}
