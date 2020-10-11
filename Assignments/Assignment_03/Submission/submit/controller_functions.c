#include "init.h"
#include "controller_functions.h"

int send_ack_pkt(int fd, char* swi){
    char ack_pkt[12];
    strcpy(ack_pkt, "ACK");
    strcat(ack_pkt, " ");
    strcat(ack_pkt, swi);
    int ret = send(fd, ack_pkt, strlen(ack_pkt), 0);
    return ret;
}

void store_switch_info(int index, char* swi, char* swj, char* swk, char* srcIP, char* desIP){
    sw_struct[index].swi        = swi;
    sw_struct[index].swj        = swj;
    sw_struct[index].swk        = swk;
    sw_struct[index].srcIP      = srcIP;
    sw_struct[index].desIP      = desIP;
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

void create_fifo_switches(int nSwitch){
    char fifo[124];
    for(int i=1; i<nSwitch+1; i++){
        if(i == 1){
            fifo_name(fifo, i, i+1);
            printf("%s\n", fifo);
            mkfifo(fifo, 0666);
        }else if(i == nSwitch){
            fifo_name(fifo, i, i+1);
            printf("%s\n", fifo);
            mkfifo(fifo, 0666);
        }else{
            fifo_name(fifo, i, i+1);
            printf("%s\n", fifo);
            mkfifo(fifo, 0666);
            fifo_name(fifo, i, i-1);
            printf("%s\n", fifo);
            mkfifo(fifo, 0666);
        }
    }
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

void print_welcome_cont(){
  printf("\n");
  printf("  ##########################################################################\n");
  printf("  #                                                                        #\n");
  printf("  #             CMPUT 379 -- a3sdn (Controller)                            #\n");
  printf("  #                                                                        #\n");
  printf("  #             (1) Please Turn on Controller                              #\n");
  printf("  #             (2) Please Turn on Switches in increasing order            #\n");
  printf("  #             (3) Please wait around 3-6 seconds to turn on              #\n");
  printf("  #                 the next switch (depending on size of trafficFile)     #\n");
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
