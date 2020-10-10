#include "init.h"
#include "controller_functions.h"

int remove_fifo(){ system("rm myfifo*"); return 0; }

int create_switch_fifo(int nSwitch){
  for(int a = 1; a < nSwitch+1 ; a++){
    if(a == 1){                           mkfifo(fifo_name(a,a+1),0666); }
    else if(a == nSwitch){                mkfifo(fifo_name(a,a-1),0666); }
    else{ mkfifo(fifo_name(a,a-1),0666);  mkfifo(fifo_name(a,a+1),0666); }}
  return 0; }

char* fifo_name(int read, int write){
  char*       final;
  char        fifo_name[12];
  char        read_char[2];
  char        write_char[2];
  sprintf(read_char,  "%d", read);  sprintf(write_char, "%d", write);
  strcpy(fifo_name, "myfifo-");     strcat(fifo_name, read_char);
  strcat(fifo_name, "-");           strcat(fifo_name, write_char);
  final = malloc(strlen(fifo_name) * sizeof(char));
  strcpy(final, fifo_name);
  return final;
}
