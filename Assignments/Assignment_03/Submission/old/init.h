#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>

#define MAX_NSW     7
#define MAXIP       1000
#define TIMEOUT     10
#define SRCIP_LO    0
#define SRCIP_HI    1000
#define UNUSED(x)   (void)(x)


//ACCESS FOR CONTROLLER

struct  pollfd  fds_cont[MAX_NSW+1];
int     fds_array[MAX_NSW+1][2];

struct switch_info{
    char*   swi;
    char*   swj;
    char*   swk;
    char*   srcIP;
    char*   desIP;
};
struct switch_info sw_struct[MAX_NSW+1];

struct controller_data{
    //Received
    int     open;
    int     query;
    //Transmitted
    int     ack;
    int     add;
};
struct controller_data cont_data;

//ACCESS FOR SWITCH

int   ft_index;

int   delay_flag;

struct switch_data{
    //Received
    int     admit;
    int     ack;
    int     addrule;
    int     relayin;
    //Transmitted
    int     open;
    int     query;
    int     relayout;
};
struct switch_data swi_data;

struct flow_table{
    int     srcIP_lo;
    int     srcIP_hi;
    int     desIP_lo;
    int     desIP_hi;
    char*   actionType;
    int     actionVal;
    int     pri;
    int     pktCount;
};
struct flow_table ft_struct[MAX_NSW+1];
