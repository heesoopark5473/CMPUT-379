#include "init.h"

int check_argv_number(int argc){
  if(argc == 3 || argc == 6) { return 1; }
  else { return -1; } }

int check_prgm(char* prgm){
  if(strncmp(prgm, "cont", 4) == 0)     { return 1; }
  else if(strncmp(prgm, "sw", 2) == 0)  { return 2; }
  else{ return -1; }
}

int check_cont_num(char* nSwitch){
  int cont_count;
  cont_count = atoi(nSwitch);
  if(cont_count > 0 && cont_count <= MAX_NSW){ return cont_count; }
  else{ return -1; }
}

int check_sw_num(char* sw){
  int sw_int;
  if(strncmp(sw, "sw", 2) == 0) {
    sw_int = atoi(strtok(sw, "sw"));
    if(sw_int > MAX_NSW || sw_int < 1) {return -1;} else { return sw_int; } }
  else if(strncmp(sw, "null", 4) == 0) {sw_int = atoi(sw); return sw_int; }
  else { return -1; }
}
