#include <stdio.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

#define SOCKET int

// Them Comment in here

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

int main(int argc, char* argv[])
{
    if(argc != 3)
    {
        setTextColor(stdout, TC_RED);
        printf("Usage: ./muti_client <ServerIp> <ServerPort> \n");
        exit(1);
    }

    // initialize sockfd_client
    setTextColor(stdout, TC_BLACK);
    printf("Creating sockfd_client... \n");
    SOCKET sockfd_client;
    sockfd_client = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd_client < 0)
    {
        setTextColor(stdout, TC_RED);
        printf("Creating sockfd_client fail ! \n");
        exit(1);
    }
    setTextColor(stdout, TC_GREEN);
    printf("Creating sockfd_client success... \n");

    // checking sockfd_client open or not 
    setTextColor(stdout, TC_BLACK);
    printf("Checking sockfd_client... \n");
    int flag;
    flag = fcntl(sockfd_client, F_GETFL, 0);
    if(flag < 0)
    {
        setTextColor(stdout, TC_RED);
        printf("Sockfd_client not open ! \n");
        exit(1);
    }
    setTextColor(stdout, TC_GREEN);
    printf("Sockfd_client already open... \n");

    // set blocking mode 
    setTextColor(stdout, TC_BLACK);
    printf("Setting blocking mode... \n");
    if(fcntl(sockfd_client, F_SETFL, flag & ~O_NONBLOCK) < 0)
    {
        setTextColor(stdout, TC_RED);
        printf("Setting blocking mode fail ! \n");
        exit(1);
    }
    setTextColor(stdout, TC_GREEN);
    printf("Setting blocking mode success... \n");

    // assign value for struct of server socket
    struct sockaddr_in addr_server;
    memset(&addr_server, 0, sizeof(addr_server));
    addr_server.sin_family = AF_INET;
    addr_server.sin_addr.s_addr = inet_addr(argv[1]);
    addr_server.sin_port = htons(atoi(argv[2]));

    // connecto socket server
    setTextColor(stdout, TC_BLACK);
    printf("Connecting to server... \n");
    if(connect(sockfd_client, (struct sockaddr*) &addr_server, sizeof(addr_server)) < 0)
    {
        setTextColor(stdout, TC_RED);
        printf("Connecting to server fail ! \n");
        exit(1);
    }
    setTextColor(stdout, TC_GREEN);
    printf("Connecting to server success... \n");
    setTextColor(stdout, TC_BLACK);
    printf("------------------- CONNECTED SUCCESS ------------------- \n");
    while(1)
    {
        char buffer_data[1024];
        printf("\nEnter data to send to server: ");
        fgets(buffer_data, 1024, stdin);
        buffer_data[strlen(buffer_data) - 1] = '\0';
        send(sockfd_client, buffer_data, sizeof(buffer_data), 0);
        if(strcmp(buffer_data, "break") == 0)
        {
            setTextColor(stdout, TC_GREEN);
            printf("Break success... \n");
            break;
        }
    }
    close(sockfd_client);
}