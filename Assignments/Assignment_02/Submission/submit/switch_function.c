#include "init.h"
#include "switch_function.h"

int file_exist(char *filename){
  struct stat   buffer;
  return (stat(filename, &buffer) == 0);
}

int get_swi_int(char* swi){
  int swi_int     = atoi(strtok(swi, "sw"));
  return swi_int;
}

int initialize_flowtable(char* IP_range){

  ft_struct[flow_table_index].srcIP_lo    = 0;
  ft_struct[flow_table_index].srcIP_hi    = MAXIP;
  ft_struct[flow_table_index].desIP_lo    = get_ip_int(IP_range, 1);
  ft_struct[flow_table_index].desIP_hi    = get_ip_int(IP_range, 2);
  ft_struct[flow_table_index].actionType  = "FORWARD";
  ft_struct[flow_table_index].actionVal   = 3;
  ft_struct[flow_table_index].pri         = 4;
  ft_struct[flow_table_index].pktCount    = 0;
  return 0;
}

int get_ip_int(char* IP_range, int index){
  char* copy = malloc(strlen(IP_range)+1);
  strcpy(copy, IP_range);
  char* low = strtok(copy, "-");
  char* high = strtok(NULL, "-");
  if(index == 1){return atoi(low);}
  else if(index == 2){return atoi(high);}
  else{return 0;}
}

char* get_open_pkt(char* swi, char* swj, char* swk, char* IP_range){
  char* open_pkt = malloc(sizeof(char)*32);
  strcat(open_pkt, "OPEN"); strcat(open_pkt, " ");
  strcat(open_pkt, swi);    strcat(open_pkt, " ");
  strcat(open_pkt, swj);    strcat(open_pkt, " ");
  strcat(open_pkt, swk);    strcat(open_pkt, " ");
  strcat(open_pkt, IP_range);
  return open_pkt;
}

char* calculate_pkt(char* trafficLine, char* swi, int LowIP, int HighIP){
  char* line_copy;
  line_copy = malloc(sizeof(char)*32);
  strcpy(line_copy, trafficLine);
  char* t0 = strtok(line_copy, " "); UNUSED(t0);
  char* t1 = strtok(NULL, " ");
  char* t2 = strtok(NULL, " ");

  if(strncmp(trafficLine, "#", 1) != 0 && strncmp(trafficLine, " ", 1) != 0 && strncmp(trafficLine, swi, 3) == 0){
    int sw_src      = atoi(t1); UNUSED(sw_src);
    int sw_dest     = atoi(t2);
    char* ret_pkt   = malloc(sizeof(char)*64);

    swi_data.admit++;

    int query_flag = 0;
    for(int e=0; e<flow_table_index+1; e++){
      if(sw_dest >= ft_struct[e].desIP_lo &&  sw_dest <= ft_struct[e].desIP_hi){
        if(strncmp(ft_struct[e].actionType, "FORWARD", 7)== 0 && strncmp(swi, "sw1", 3) != 0 && e != 0){
          ft_struct[e].pktCount++;
          char actionval[2];
          sprintf(actionval, "%d", ft_struct[e].actionVal);
          strcat(ret_pkt, "RELAY");
          strcat(ret_pkt, " ");
          strcat(ret_pkt, t2);
          strcat(ret_pkt, " ");
          strcat(ret_pkt, actionval); 
          return ret_pkt; 
        }
        ft_struct[e].pktCount++;
        query_flag = 1;
      }else if(sw_dest < ft_struct[e].desIP_lo || sw_dest > ft_struct[e].desIP_hi){
      }
    }

    if(query_flag > 0){
      strcat(ret_pkt, "ADD");
      strcat(ret_pkt, " ");
      strcat(ret_pkt, swi);
      strcat(ret_pkt, " ");
      strcat(ret_pkt, t1);
      strcat(ret_pkt, " ");
      strcat(ret_pkt, t2);   
      return ret_pkt;

    }else if(query_flag == 0){
      strcat(ret_pkt, "QUERY");
      strcat(ret_pkt, " ");
      strcat(ret_pkt, swi);
      strcat(ret_pkt, " ");
      strcat(ret_pkt, t1);
      strcat(ret_pkt, " ");
      strcat(ret_pkt, t2);      

      return ret_pkt;
    }
  }
  return "NULL";
}

int add_drop_rule(char* t1, int flow_table_index){
  ft_struct[flow_table_index].srcIP_lo    = 0;
  ft_struct[flow_table_index].srcIP_hi    = MAXIP;
  ft_struct[flow_table_index].desIP_lo    = atoi(t1);
  ft_struct[flow_table_index].desIP_hi    = atoi(t1);
  ft_struct[flow_table_index].actionType  = "DROP";
  ft_struct[flow_table_index].actionVal   = 0;
  ft_struct[flow_table_index].pri         = 4;
  ft_struct[flow_table_index].pktCount++;
  return 0;
}

int print_list_quit_swi(int flow_table_index){
  char input[1024];
  fgets(input, sizeof input / sizeof *input, stdin);
  if (strncmp(input, "list", 4) == 0) {
      printf("\nFlow Table:\n");
      for(int p=0; p<flow_table_index+1; p++){
          printf("[%d] (srcIP= %d-%d, destIP= %d-%d, action= %s: %d, pri= %d, pktCount= %d)\n", p, ft_struct[p].srcIP_lo, ft_struct[p].srcIP_hi, ft_struct[p].desIP_lo, ft_struct[p].desIP_hi, ft_struct[p].actionType, ft_struct[p].actionVal, ft_struct[p].pri, ft_struct[p].pktCount);
      }
      printf(" \n");
      printf("Packet Stats:\n");
      printf("        Received    :   ADMIT:%d,    ACK:%d,     ADDRULE:%d,      RELAYIN:%d\n", swi_data.admit, swi_data.ack, swi_data.addrule, swi_data.relayin);
      printf("        Transmitted :   OPEN :%d,   QUERY:%d,    RELAYOUT:%d\n", swi_data.open, swi_data.query, swi_data.relayout);
  } else if (strncmp(input, "exit", 4) == 0) {
      printf("\nFlow Table:\n");
      for(int p=0; p<flow_table_index+1; p++){
          printf("[%d] (srcIP= %d-%d, destIP= %d-%d, action= %s: %d, pri= %d, pktCount= %d)\n", p, ft_struct[p].srcIP_lo, ft_struct[p].srcIP_hi, ft_struct[p].desIP_lo, ft_struct[p].desIP_hi, ft_struct[p].actionType, ft_struct[p].actionVal, ft_struct[p].pri, ft_struct[p].pktCount);
      }
      printf(" \n");
      printf("Packet Stats:\n");
      printf("        Received    :   ADMIT:%d,    ACK:%d,     ADDRULE:%d,      RELAYIN:%d\n", swi_data.admit, swi_data.ack, swi_data.addrule, swi_data.relayin);
      printf("        Transmitted :   OPEN :%d,   QUERY:%d,    RELAYOUT:%d\n", swi_data.open, swi_data.query, swi_data.relayout);
      exit(1);
  }
  return 0;
}

