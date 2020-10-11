#include "init.h"
#include "controller_functions.h"

int send_ack_pkt(int fd, char* swi){
    char ack_pkt[32];
    strcpy(ack_pkt, "ACK");
    strcat(ack_pkt, " ");
    strcat(ack_pkt, swi);
    int ret = write(fd, ack_pkt, strlen(ack_pkt));
    return ret;
}

int send_add_pkt(int fd, char* srcIP, char* desIP){
    char add_pkt[64] = {0};
    strcpy(add_pkt, "ADDRULE");   strcat(add_pkt, " ");
    strcat(add_pkt, srcIP);       strcat(add_pkt, " ");
    strcat(add_pkt, desIP);
    int ret = write(fd, add_pkt, sizeof(add_pkt));
    return ret;
}

int send_relay_pkt(int fd, char* ret_swi, char* srcIP, char* desIP, char* IPlo, char* IPhi, char* swi){
  char relay_pkt[64] = {0};
  strcpy(relay_pkt, "RELAY");     strcat(relay_pkt, " ");
  strcat(relay_pkt, ret_swi);     strcat(relay_pkt, " ");
  strcat(relay_pkt, srcIP);        strcat(relay_pkt, " ");
  strcat(relay_pkt, desIP);        strcat(relay_pkt, " ");
  strcat(relay_pkt, IPlo);       strcat(relay_pkt, " ");
  strcat(relay_pkt, IPhi);       strcat(relay_pkt, " ");
  strcat(relay_pkt, swi);   
  int ret = write(fd, relay_pkt, sizeof(relay_pkt));
  return ret;
}

void store_switch_info(int index, char* swi, char* swj, char* swk, char* srcIP, char* desIP){

    char copy_swi[12], copy_swj[12], copy_swk[12], copy_srcIP[12], copy_desIP[12];

    strcpy(copy_swi,    swi);
    strcpy(copy_swj,    swj);
    strcpy(copy_swk,    swk);
    strcpy(copy_srcIP,  srcIP);
    strcpy(copy_desIP,  desIP);

    strcpy(sw_struct[index].swi,    copy_swi);
    strcpy(sw_struct[index].swj,    copy_swj);
    strcpy(sw_struct[index].swk,    copy_swk);
    strcpy(sw_struct[index].srcIP,  copy_srcIP);
    strcpy(sw_struct[index].desIP,  copy_desIP);
}

void *socket_accept(void *ptr){
    int counter = 0;
    char* portNumber;
    portNumber = (char *)ptr;
    int listenfd = 0, connfd = 0;
    struct sockaddr_in serv_addr;
    char sendBuff[1025];
    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    memset(&serv_addr, '0', sizeof(serv_addr));
    memset(sendBuff, '0', sizeof(sendBuff));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(atoi(portNumber));
    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr));
    listen(listenfd, 10);
    while(1){
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        fds_cont[counter].fd     = connfd;
        fds_cont[counter].events = POLLIN;

        fds_array[counter][0]  = connfd;
        fds_array[counter][1]  = POLLIN;
        counter++;
    }
    return NULL;
}


void fifo_name(char buf[], int read, int write){
    char    read_char[32];
    char    write_char[32];
    sprintf(read_char, "%d", read);
    sprintf(write_char, "%d", write);
    strcpy(buf, "myfifo-");
    strcat(buf, read_char);
    strcat(buf, "-");
    strcat(buf, write_char);
}

void print_user_commnad_cont(int nSwitch){
  char input[1024];
  fgets(input, sizeof input / sizeof *input, stdin);
  if(strncmp(input, "list", 4) == 0 || strncmp(input, "exit", 4) == 0){
    printf("\nSwitch Information (nSwitch: %d): \n\n", nSwitch);
    for(int a=1; a<sw_index+1; a++){
        printf("[sw%s] port1= %s, port2= %s, port3= %s-%s\n", sw_struct[a].swi,sw_struct[a].swj, sw_struct[a].swk, sw_struct[a].srcIP, sw_struct[a].desIP);
    }
    printf("\nPacket Stats: \n");
    printf("\tReceived    :     OPEN: %d, QUERY: %d\n", cont_data.open, cont_data.query);
    printf("\tTransmitted :     ACK : %d, ADD  : %d\n\n", cont_data.ack, cont_data.add);
  }else{printf("\nInvalid User Command Entered\n\n");}
  if(strncmp(input, "exit", 4) == 0){exit(0);}
}

void print_welcome_cont(){
  printf("\n");
  printf("  ##########################################################################\n");
  printf("  #                                                                        #\n");
  printf("  #             CMPUT 379 -- a3sdn (Controller)                            #\n");
  printf("  #                                                                        #\n");
  printf("  #             (1) Please Turn on Controller                              #\n");
  printf("  #             (2) Please Turn on Switches in increasing order            #\n");
  printf("  #             (4) Type 'list' to view the status                         #\n");
  printf("  #             (5) Type 'quit' to view the status and terminate           #\n");
  printf("  #                                                                        #\n");
  printf("  #                                                                        #\n");
  printf("  #              User Command List:                                        #\n");
  printf("  #             (1) list - View the status of Contorller or Switch         #\n");
  printf("  #             (2) exit - View teh status and terminate                   #\n");
  printf("  #                                                                        #\n");
  printf("  #                                                                        #\n");
  printf("  #                 Thank you -by Hee Soo Park                             #\n");
  printf("  #                                                                        #\n");
  printf("  ##########################################################################\n");
  printf("\n\n");
  printf("  ##########################################################################\n");
  printf("  #                                                                        #\n");
  printf("  #                           CONTROLLER                                   #\n");
  printf("  #                                                                        #\n");
  printf("  ##########################################################################\n");
  printf("\n\n");
}
