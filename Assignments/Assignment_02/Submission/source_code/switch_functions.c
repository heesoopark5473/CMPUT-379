#include "init.h"
#include "switch_structs.h"

int file_exist(char *filename){
  struct stat   buffer;
  return (stat(filename, &buffer) == 0);
}

int get_swi_int(char* swi){
  int swi_int     = atoi(strtok(swi, "sw"));
  return swi_int;
}

int get_ip_int(char* IP_range, int index){
  char copy[32];
  strcpy(copy, IP_range);
  char* low  = strtok(copy, "-");
  char* high = strtok(NULL, "-");
  if(index == 1){ return atoi(low); }
  else if(index == 2){ return atoi(high); }
  else{ return 0; }
}

int update_flowtable(int index, int srcIP_lo, int srcIP_hi, char* IP_range, char* actionType, int actionVal, int pri, int pktCount){
  ft_struct[index].srcIP_lo    = srcIP_lo;
  ft_struct[index].srcIP_hi    = srcIP_hi;
  ft_struct[index].desIP_lo    = get_ip_int(IP_range, 1);
  ft_struct[index].desIP_hi    = get_ip_int(IP_range, 2);
  ft_struct[index].actionType  = actionType;
  ft_struct[index].actionVal   = actionVal;
  ft_struct[index].pri         = pri;
  ft_struct[index].pktCount    = pktCount;
  return 0;
}

char* get_open_pkt(char* swi, char* swj, char* swk, char* IP_range){
  char* final;
  final = malloc(32 * sizeof(char));
  char open_pkt[24];
  strcpy(open_pkt, "OPEN"); strcat(open_pkt, " ");
  strcat(open_pkt, swi);    strcat(open_pkt, " ");
  strcat(open_pkt, swj);    strcat(open_pkt, " ");
  strcat(open_pkt, swk);    strcat(open_pkt, " ");
  strcat(open_pkt, IP_range);
  strcpy(final, open_pkt);
  return final;
}
