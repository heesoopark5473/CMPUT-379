#include "init.h"
#include "switch_functions.h"

void calculate_pkt(char buf[], char* pkt){
    char* swi     = strtok(pkt, " ");

    char* token   = strtok(NULL, " ");

    if(strncmp(token, "delay", 5) == 0){
      char* delay = "delay";
      char* delay_time = strtok(NULL, " ");

      printf("%s  %s\n", delay, delay_time);

      pthread_t thread_delay;
      int iret2 = pthread_create(&thread_delay, NULL, switch_delay, (void*) delay_time);

    }else{
      /////////////////
      swi_data.admit++;
      /////////////////
      int sw_src  = atoi(token);
      int sw_des  = atoi(strtok(NULL, " "));
      printf("%d  %d\n", sw_src, sw_des);

      for(int k=0; k<ft_index+1; k++){
        if(sw_des >= ft_struct[k].desIP_lo && sw_des <= ft_struct[k].desIP_hi && strncmp(ft_struct[k].actionType, "FORWARD", 7) == 0){
          ft_struct[k].pktCount++;
        }else if(sw_des < ft_struct[k].desIP_lo || sw_des > ft_struct[k].desIP_hi){
          swi_data.query++;
        }
      }

    }
}

void* switch_delay(void *ptr){
    char* delay_char;
    delay_char = (char *)ptr;
    int delay_time = atoi(delay_char);

    delay_flag = -1;

    printf("\n\t** Entering a delay period of %d mesc\n\n", delay_time);
    usleep(delay_time);
    printf("\n\t** Delay period ended\n\n");

    delay_flag = 0;

    return NULL;
}

void update_flowtable(int srcIP_lo, int srcIP_hi, int desIP_lo, int desIP_hi, char* actionType, int actionVal, int pri, int pktCount){
    ft_struct[ft_index].srcIP_lo    = srcIP_lo;
    ft_struct[ft_index].srcIP_hi    = srcIP_hi;
    ft_struct[ft_index].desIP_lo    = desIP_lo;
    ft_struct[ft_index].desIP_hi    = desIP_hi;
    ft_struct[ft_index].actionType  = actionType;
    ft_struct[ft_index].actionVal   = actionVal;
    ft_struct[ft_index].pri         = pri;
    ft_struct[ft_index].pktCount    = pktCount;
}

int send_open_pkt(int fd, char* swi, char* swj, char* swk, char* IP_range){
    char buffer[128];
    strcpy(buffer, "OPEN"); strcat(buffer, " ");
    strcat(buffer, swi);    strcat(buffer, " ");
    strcat(buffer, swj);    strcat(buffer, " ");
    strcat(buffer, swk);    strcat(buffer, " ");
    strcat(buffer, IP_range);
    int ret = send(fd, buffer, strlen(buffer), 0);
    return ret;
}

int file_exist(char* filename){
  struct stat buffer;
  return(stat(filename, &buffer) == 0);
}

int socket_connect(char* serverAddress, char* portNumber){
  //SOCKET START
  int sockfd = 0;
  char recvBuff[1024];
  struct sockaddr_in serv_addr;

  memset(recvBuff, '0',sizeof(recvBuff));
  if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0)
  {printf("\n Error : Could not create socket \n"); return 1;}

  memset(&serv_addr, '0', sizeof(serv_addr));

  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(atoi(portNumber));

  if(inet_pton(AF_INET, serverAddress, &serv_addr.sin_addr)<=0)
  {printf("\n inet_pton error occured\n"); return 1;}

  if(connect(sockfd,(struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
  {printf("\n Error : Connect Failed \n"); return 1;}
  //SOCKET END
  return sockfd;
}

void print_user_command(){
  char input[1024];
  fgets(input, sizeof input / sizeof *input, stdin);
  if(strncmp(input, "list", 4) == 0){
      printf("\nFlow Table:\n");
      for(int x=0; x<ft_index+1; x++){
          printf("[%d] (srcIP= %d-%d, destIP= %d-%d, action= %s: %d, pri= %d, pktCount= %d)\n", x, ft_struct[x].srcIP_lo, ft_struct[x].srcIP_hi, ft_struct[x].desIP_lo, ft_struct[x].desIP_hi, ft_struct[x].actionType, ft_struct[x].actionVal, ft_struct[x].pri, ft_struct[x].pktCount);
      }
      printf(" \n");
      printf("Packet Stats:\n");
      printf("        Received    :   ADMIT:%d,    ACK:%d,     ADDRULE:%d,      RELAYIN:%d\n", swi_data.admit, swi_data.ack, swi_data.addrule, swi_data.relayin);
      printf("        Transmitted :   OPEN :%d,   QUERY:%d,    RELAYOUT:%d\n", swi_data.open, swi_data.query, swi_data.relayout);
  }else if(strncmp(input, "exit", 4) == 0){
      printf("\nFlow Table:\n");
      for(int y=0; y<ft_index+1; y++){
          printf("[%d] (srcIP= %d-%d, destIP= %d-%d, action= %s: %d, pri= %d, pktCount= %d)\n", y, ft_struct[y].srcIP_lo, ft_struct[y].srcIP_hi, ft_struct[y].desIP_lo, ft_struct[y].desIP_hi, ft_struct[y].actionType, ft_struct[y].actionVal, ft_struct[y].pri, ft_struct[y].pktCount);
      }
      printf(" \n");
      printf("Packet Stats:\n");
      printf("        Received    :   ADMIT:%d,    ACK:%d,     ADDRULE:%d,      RELAYIN:%d\n", swi_data.admit, swi_data.ack, swi_data.addrule, swi_data.relayin);
      printf("        Transmitted :   OPEN :%d,   QUERY:%d,    RELAYOUT:%d\n", swi_data.open, swi_data.query, swi_data.relayout);
      exit(1);
  }else{printf("\nInvalid User Command Entered\n\n");}
}
