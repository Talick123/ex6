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


#define ARGC_SIZE 5

const int LEN_STR_MAX = 100;

// --------prototype section------------------------

void check_argc(int argc);
int connect_to_socket(char ip[], char port[], struct addrinfo con_kind, struct addrinfo *addr_info_res);


// --------main section------------------------

int main(int argc, char *argv[])
{
    int my_socket_p, my_socket_q;
    struct addrinfo con_kind, *addr_info_res = NULL;

    check_argc(argc);
    srand(atoi(argv[1]));
    srand(atoi(argv[3]));

    my_socket_p = connect_to_socket(argv[2], argv[3], con_kind, addr_info_res);
    my_socket_q = connect_to_socket(argv[4], argv[5], con_kind, addr_info_res);

    get_user_req(my_socket_p, my_socket_q);
    close(my_socket);
    freeaddrinfo(addr_info_res);

    return EXIT_SUCCESS;
}
//-------------------------------------------------

void get_user_req(my_socket_p, my_socket_q)
{
    int rc;
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
            default:
                break;
        }
    }
}

//------------------------------------------------

void p_request(int my_socket_p)
{
    int num, res;
    int rc;

    scanf(" %d", &num);
    getchar();

    shm_ptr_p[P_NUM] = num;

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

void q_request()
{
    int num, i;
    char str[LEN_STR_MAX];
    char c;
    for(i = 0; i < LEN_STR_MAX; i++)
    {
        c = getchar();
        //N: read to '0' ?
        if(c == '0')
        {
            str[i] = '\0'; // null?
            break;
        }
        else
            str[i] = c;
    }
    getchar();

    write(my_socket_q ,&str ,sizeof(char)*LEN_STR_MAX);
    rc = read(my_socket_q ,res ,sizeof(int)*MAX_AMOUNT);
    if(rc > 0)
    {
        printf("is palindrome ? %s\n", res ? "yes" : "no");
    }
    else
    {
        perror("read() failed");
        exit(EXIT_FAILURE);
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

	printf("Port is: %s\n", port);
	printf("IP is: %s\n", ip);

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




//
// #include <sys/types.h>
// #include <sys/socket.h>
// #include <netdb.h>
//
//
// int main(int argc, char *argv[])
// {
//     int rc;
//     int my_socket;
//     // int i, num // ?
//     char rbuf[BUFLEN];
//     struct addrinfo con_kind, *addr_info_res;
//
//     //check argc
//     if(argc < 2)
//     {
// 		fprintf(stderr, "Missing");
// 		exit(EXIT_FAILURE);
// 	}
//
//     memset(&con_kind, 0, size(con_kind));
//     con_kind.ai_family = AF_UNSPEC;
//     con_kind.ai_socktype = SOCK_STREAM;
//
//     if((rc = getaddrinfor(/*SERVER IP*/, /*SERVER PORT*/, &con_kind, &addr_info_res)!= 0))
//     {
// 		perrorandexit();
// 	}
//
//     my_socket = socket(addr_info_res->ai_family, addr_info_res->ai_socktype, addr_info_res->ai_protocol);
//
//     if(my_socket < 0)
//         perrorandexit();
//
//     rc = connect(my_socket, addr_info_res->ai_addr, addr_info_res->ai_addrlen);
//
//     if(rc)
//         perrorandexit();
//
// 	while(1)
// 	{
// 		puts("enter a number");
// 		scanf("%d", &num);
// 		if(!num) break;
//
// 		write(my_socket, &num, rbuf, BUFLEN);
// 		rc = read(my_socket, rbuf, BUFLEN);
//
// 		if(rc > 0)
// 			printf("%s\n", rbuf);
// 		else
// 		{
// 			perrorandexit() // read failed
// 		}
// 	}
//
// 	close(my_soscket);
// 	freeaddrinfo(addr_info_res);
//
//     return EXIT_SUCCESS;
// }
