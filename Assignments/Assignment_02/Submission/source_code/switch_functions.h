int     update_flowtable(int index, int srcIP_lo, int srcIP_hi, char* IP_range, char* actionType, int actionVal, int pri, int pktCount);
char*   get_open_pkt(char* swi, char* swj, char* swk, char* IP_range);
int     file_exist(char* filename);
int     get_swi_int(char* swi);

int     get_ip_int(char* IP_range, int index);
char*   calculate_pkt(char* trafficLine, char* swi, int LowIP, int HighIP);
int     print_list_quit_swi(int flow_table_index);
int     add_drop_rule(char* t1, int flow_table_index);
