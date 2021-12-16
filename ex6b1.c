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
