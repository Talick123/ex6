/*

- via argument vector gets number (1, 2 or 3) and address of computer and port
    - looks like this: ./producer 1 localhost 17171
- uses atoi(argv[1]) as seed
- connects to server via socket
- in a loop:
    - generates random number
    - checks if prime
    - sends prime to server
    - receives feedback
        - feedback is either # of times the server received the number sent
        - OR: -1 (which means array is full and it didnt read the number sent)

- to start, client sends message to server that its ready, waits for answer to start
- when done, prints output like last time

*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
