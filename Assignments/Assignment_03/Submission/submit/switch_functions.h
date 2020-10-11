int     send_open_pkt(int sockfd, char* swi, char* swj, char* swk, char* IPlow, char* IPhigh);
int     file_exist(char* filename);
int     socket_connect(char* serverAddress, char* portNumber);
void    update_flowtable(int srcIP_lo, int srcIP_hi, int desIP_lo, int desIP_hi, char* actionType, int actionVal, int pri, int pktCount);
void    print_user_command_switch();
int     calculate_pkt(char buf[], char* pkt);
void*   switch_delay(void *ptr);
