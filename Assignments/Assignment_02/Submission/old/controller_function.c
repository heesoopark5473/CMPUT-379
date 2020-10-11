#include "init.h"
#include "controller_function.h"

// create FIFOs between switches for between switch communication
int create_switch_fifo(int nSwitch){
  for(int a = 1; a < nSwitch+1 ; a++){
    if(a == 1){
      mkfifo(fifo_name(a,a+1),0666);
    }else if(a == nSwitch){
      mkfifo(fifo_name(a,a-1),0666);
    }else{
      mkfifo(fifo_name(a,a-1),0666);
      mkfifo(fifo_name(a,a+1),0666);
    }
  }
  return 0;
}

// return the file name of FIFO, from (2 switches) or (1 switch & 1 controller) number that wants to communicate
char* fifo_name(int read, int write){
  char      read_char[32];
  char      write_char[32];
  char*     final = malloc(sizeof(char)*32);

  sprintf(read_char,  "%d", read); 
  sprintf(write_char, "%d", write);

  strcat(final, "myfifo-");   
  strcat(final, read_char);
  strcat(final, "-");         
  strcat(final, write_char);
  return final;
}

// once switch has been turned on and controller receives the open packet
// controller stores that switch information in a struct array
char* store_swi_info(char* message){
  char*   t1 = strtok(message, " "); UNUSED(t1);
  char*   t2 = strtok(NULL, " ");   char*   t3 = strtok(NULL, " ");
  char*   t4 = strtok(NULL, " ");   char*   t5 = strtok(NULL, " ");

  if(strncmp(t3, "null", 4) == 0){strcpy(t3, "-1");}
  if(strncmp(t4, "null", 4) == 0){strcpy(t4, "-1");}

  char* i_t2      = strtok(t2, "sw");
  char* i_t3      = strtok(t3, "sw");
  char* i_t4      = strtok(t4, "sw");

  int swi_int     = atoi(i_t2);

  sw_struct[swi_int].swi      = malloc(strlen(t2)+1);
  sw_struct[swi_int].swj      = malloc(strlen(t3)+1);
  sw_struct[swi_int].swk      = malloc(strlen(t4)+1);
  sw_struct[swi_int].IP_range = malloc(strlen(t5)+1);

  strcpy(sw_struct[swi_int].swi, i_t2);
  strcpy(sw_struct[swi_int].swj, i_t3);
  strcpy(sw_struct[swi_int].swk, i_t4);
  strcpy(sw_struct[swi_int].IP_range, t5);

  return t2;
}

// print the list or exit upon users request
int print_list_quit_cont(int nSwitch){
  char input[1024];
  fgets(input, sizeof input / sizeof *input, stdin);
  if (strncmp(input, "list", 4) == 0) {
      printf("\nSwitch Information (nSwitch %d): \n", nSwitch);
      for(int i=1; i<MAX_NSW+1; i++){
          if(sw_struct[i].swi != NULL){
              printf("[sw%s] port1 = %s, port2 = %s, port3 = %s\n", sw_struct[i].swi, sw_struct[i].swj, sw_struct[i].swk, sw_struct[i].IP_range);
          }
      }
      printf("Packet Stats: \n");
      printf("        Received    :   OPEN: %d, QUERY: %d\n", cont_data.open, cont_data.query);
      printf("        Transmitted :   ACK : %d, ADD  : %d\n", cont_data.ack, cont_data.add);
  } else if (strncmp(input, "exit", 4) == 0) {
      printf("\nSwitch Information (nSwitch %d): \n", nSwitch);
      for(int i=1; i<MAX_NSW+1; i++){
          if(sw_struct[i].swi != NULL){
              printf("[sw%s] port1 = %s, port2 = %s, port3 = %s\n", sw_struct[i].swi, sw_struct[i].swj, sw_struct[i].swk, sw_struct[i].IP_range);
          }
      }
      printf("Packet Stats: \n");
      printf("        Received    :   OPEN: %d, QUERY: %d\n", cont_data.open, cont_data.query);
      printf("        Transmitted :   ACK : %d, ADD  : %d\n", cont_data.ack, cont_data.add);
      exit(0);
  }
  return 0;
}
