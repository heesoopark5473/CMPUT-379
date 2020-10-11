void print_welcome_cont();
void create_fifo_switches(int nSwitch);
void fifo_name(char buf[], int read, int write);
void *socket_accept();
void store_switch_info(int index, char* swi, char* swj, char* swk, char* srcIP, char* desIP);
int send_ack_pkt(int fd, char* swi);
