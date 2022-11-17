#include <stdio.h>          // printf...
#include <stdlib.h>         // exit, srand()...
#include <arpa/inet.h>      // htons...
#include <netinet/in.h>     // struct sockaddr_in...
#include <sys/socket.h>     // socket...
#include <string.h>         // memset..
#include <fcntl.h>          // fcntl...
#include <unistd.h>         // close...
#include <pthread.h>        // thread
#include <errno.h>          // errno
#include <time.h>           //time

#define SOCKET int
#define MAX_CLIENT 10

// funtion to set color 
typedef enum TextColor
{
    TC_BLACK = 0,
    TC_BLUE = 1,
    TC_GREEN = 2,
    TC_CYAN = 3,
    TC_RED = 4,
    TC_MAGENTA = 5,
    TC_BROWN = 6,
    TC_LIGHTGRAY = 7,
    TC_DARKGRAY = 8,
    TC_LIGHTBLUE = 9,
    TC_LIGHTGREEN = 10,
    TC_LIGHTCYAN = 11,
    TC_LIGHTRED = 12,
    TC_LIGHTMAGENTA = 13,
    TC_YELLOW = 14,
    TC_WHITE = 15
} TextColor;

static const char *ansiColorSequences[] =
{
    "\x1B[0;30m",
    "\x1B[0;34m",
    "\x1B[0;32m",
    "\x1B[0;36m",
    "\x1B[0;31m",
    "\x1B[0;35m",
    "\x1B[0;33m",
    "\x1B[0;37m",
    "\x1B[1;30m",
    "\x1B[1;34m",
    "\x1B[1;32m",
    "\x1B[1;36m",
    "\x1B[1;31m",
    "\x1B[1;35m",
    "\x1B[1;33m",
    "\x1B[1;37m"
};

static const char *ansiColorTerms[] =
{
    "xterm",
    "rxvt",
    "vt100",
    "linux",
    "screen",
    0
    // there are probably missing a few others
};

// get current terminal and check whether it's in our list of terminals
// supporting ANSI colors:
static int isAnsiColorTerm(void)
{
    char *term = getenv("TERM");
    for (const char **ansiTerm = &ansiColorTerms[0]; *ansiTerm; ++ansiTerm)
    {
        int match = 1;
        const char *t = term;
        const char *a = *ansiTerm;
        while (*a && *t)
        {
            if (*a++ != *t++)
            {
                match = 0;
                break;
            }
        }
        if (match) return 1;
    }
    return 0;
}

void setTextColor(FILE *stream, TextColor color)
{
    int outfd = fileno(stream);
    if (isatty(outfd) && isAnsiColorTerm())
    {
        // we're directly outputting to a terminal supporting ANSI colors,
        // so send the appropriate sequence:
        fputs(ansiColorSequences[color], stream);
    }
}


// define struct to contain both sockfd and any para to cast void to get int
typedef struct server_id{
    SOCKET sockfdd_server;
    int id;
} server_id;

void *server_recv(void *server_sockfd);
int main(int argc, char* argv[])
{
    srand((unsigned int)time(NULL));
    // check input argument
    if(argc != 3)
    {
        // set text color
        setTextColor(stdout, TC_RED);
        printf("Usage: ./muti_server <ServerIp> <ServerPort> \n");
        exit(1);
    }

    // declare sockfd_server
    SOCKET sockfd_server;
    // socket function
    sockfd_server = socket(AF_INET, SOCK_STREAM, 0);
    setTextColor(stdout, TC_BLACK);
    printf("Creating sockfd_server... \n");

    // checking socket fail?  
    if(sockfd_server < 0)
    {
        printf("Creating socfd_server fail !\n");
        exit(1);
    }

    // Setting option REUSEADDR
    printf("Setting REUSEADDR option... \n");
    int opt = 1;
    if(setsockopt(sockfd_server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
    {
        printf("Setting REUSEADDR fail... \n");
        exit(1);
    }
    printf("Setting REUSEADDR success... \n");
    setTextColor(stdout, TC_GREEN);

    // creating success
    printf("Creating sockfd_server success... \n");
    setTextColor(stdout, TC_BLACK);
    
    // checking sockfd_server open or not, and set blocking mode for sockfd_server
    printf("Checking socfd_server open... \n");
    int flag = fcntl(sockfd_server, F_GETFL, 0);
    if(flag < 0)
    {
        setTextColor(stdout, TC_RED);
        printf("Sockfd_server not open !\n");
        exit(1);
    }
    setTextColor(stdout, TC_GREEN);
    // open success
    printf("Sockfd_server open success... \n");

    // set blocking mode
    setTextColor(stdout, TC_BLACK);
    printf("Setting blocking mode... \n");
    if(fcntl(sockfd_server, F_SETFL, flag & ~O_NONBLOCK) < 0)
    {
        setTextColor(stdout, TC_RED);
        printf("Setting blocking mode fail ! \n");
        exit(1);
    }
    // setting success
    setTextColor(stdout, TC_GREEN);
    printf("Setting blocking mode success... \n");
    
    // declare struct contain IP and Port of server
    struct sockaddr_in addr_server;
    
    memset(&addr_server, 0, sizeof(addr_server));
    addr_server.sin_family = AF_INET;
    addr_server.sin_addr.s_addr = inet_addr(argv[1]);
    addr_server.sin_port = htons(atoi(argv[2]));

    // bind sockfd_server
    setTextColor(stdout, TC_BLACK);
    printf("Binding socket... \n");
    if(bind(sockfd_server, (struct sockaddr*) &addr_server, sizeof(addr_server)) < 0)
    {   
        setTextColor(stdout, TC_RED);
        printf("Binding socket fail ! \n");
        exit(1);
    }
    setTextColor(stdout, TC_GREEN);
    printf("Binding sockfd_server success... \n");

    // set listen max client
    setTextColor(stdout, TC_BLACK);
    printf("Setting listening... \n");
    if(listen(sockfd_server, MAX_CLIENT) < 0)
    {
        setTextColor(stdout, TC_RED);
        printf("Setting listening fail ! \n");
        exit(1);
    }
    setTextColor(stdout, TC_GREEN);
    printf("Setting listening success... \n");

    pthread_t thread_id;
    // initialize array of server
    server_id ser[MAX_CLIENT];
    for(int i = 0 ; i < MAX_CLIENT; i++)
    {
        ser[i].id = 1000 + rand() % (9999 + 1 - 1000);
        ser[i].sockfdd_server = sockfd_server;
    }

    // muti client or thread
    for(int i = 0; i < MAX_CLIENT; i++)
    {
        pthread_create(&thread_id, NULL, server_recv, (void*) (ser + i));
    }
    pthread_exit(NULL);
    
    return 0;
}


void *server_recv(void *serverr_sockfd)
{
    // struct to contain Ip and port of client
    struct sockaddr_in client_addr;
    // sockfd of client
    SOCKET sockfd_client;
    // get sockfd_server
    int length = sizeof(client_addr);
    sockfd_client = accept((((server_id *)serverr_sockfd)->sockfdd_server), (struct sockaddr*) &client_addr, &length);
    if(sockfd_client < 0)
    {
        setTextColor(stdout, TC_RED);
        printf("Connecting fail ! \n");
        printf("%d", errno);
        exit(1);
    }
    // buffer contain IP client
    char buffer[256];
    // contain port client
    int port;
    // inet_ntop function
    inet_ntop(client_addr.sin_family, &client_addr.sin_addr.s_addr, buffer, 256);
    // ntohs function
    port = ntohs(client_addr.sin_port);

    // printf Connecting success 
    setTextColor(stdout, TC_BLACK);
    printf("Connecting to ");
    setTextColor(stdout, TC_GREEN);
    printf("%s %d... \n", buffer, port);
    setTextColor(stdout, TC_BLACK);
    while(1)
    {
        char buffer_data[1024];
        recv(sockfd_client, buffer_data, 1024, 0);
        printf("Data from ");
        setTextColor(stdout, TC_GREEN);
        printf("%s %d", buffer, port);
        setTextColor(stdout, TC_BLACK);
        printf(": %s \n", buffer_data);
        if(strcmp(buffer_data, "break") == 0)
        {
            printf("Connection Closed to ");
            setTextColor(stdout, TC_GREEN);
            printf("%s %d \n", buffer, port);
            exit(1);
        }
    }
    close(sockfd_client);
}

