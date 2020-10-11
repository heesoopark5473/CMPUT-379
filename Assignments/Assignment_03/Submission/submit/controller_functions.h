void    print_welcome_cont();
// void    create_fifo_switches(int nSwitch);
void    fifo_name(char buf[], int read, int write);
void    print_user_commnad_cont(int nSwitch);
void    store_switch_info(int index, char* swi, char* swj, char* swk, char* srcIP, char* desIP);
void*   socket_accept();
int     send_ack_pkt(int fd, char* swi);
int     send_add_pkt(int fd, char* srcIP, char* desIP);
int     send_relay_pkt(int fd, char* ret_swi,char* srcIP, char* desIP, char* IPlo, char* IPhi, char* swi);
