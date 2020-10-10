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

int flow_table_index;
