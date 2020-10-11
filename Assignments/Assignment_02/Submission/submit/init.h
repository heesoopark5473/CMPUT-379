#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_NSW 7
#define MAXIP 1000
#define TIMEOUT 100
#define SRCIP_LO 0
#define SRCIP_HI 1000
#define UNUSED(x) (void)(x)

int flow_table_index;

// ACCESS FOR CONTROLLER
struct switch_info {
    char*   swi;
    char*   swj;
    char*   swk;
    char*   IP_range;
};
struct switch_info sw_struct[MAX_NSW+1];

struct controller_data {
  //Received
  int     open;
  int     query;
  //Transmitted
  int     ack;
  int     add;
};
struct controller_data cont_data;


// ACCESS FOR SWITCH
struct switch_data {
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


struct flow_table {
    int     srcIP_lo;
    int     srcIP_hi;
    int     desIP_lo;
    int     desIP_hi;
    char*   actionType;
    int     actionVal;
    int     pri;
    int     pktCount;
};
struct flow_table ft_struct[200];
