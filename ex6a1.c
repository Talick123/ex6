/*
- receives port via argument vector
- waits for 3 messages via socket, sends back 3 to tell clients to start

- defines array of integer ARR_SIZE = 1000
- from each client, reads number, adds to array
- sends feedback to client that sent the number
- when finished to fill array, sends to clients -1
- Output: same as previous

inferno-03 = 10.3.10.25
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>

int main(/*arc*/)
{

    int rc;
    int my_socket;
    char rbuf[BUFLEN];
    struct addrinfo con_kind, *addr_info_res;

    //check argc

    memset(&con_kind, 0, size(con_kind));
    con_kind.ai_family = AF_UNSPEC;
    con_kind.ai_socktype = SOCK_STREAM;

    if((rc = getaddrinfor(/*SERVER IP*/, /*SERVER PORT*/, &con_kind, &addr_info_res)!= 0))
        perrorandexit();

    my_socket = socket(addr_info_res->ai_family, addr_info_res->ai_socktype, addr_info_res->ai_protocol);

    if(my_socket < 0)
        perrorandexit();

    rc = connect(my_socket, addr_info_res->ai_addr, addr_info_res->ai_addrlen);

    if(rc)
        perrorandexit();

    
    return EXIT_SUCCESS;
}
