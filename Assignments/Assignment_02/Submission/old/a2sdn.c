//Include necessary header files
#include "main_function.h"
#include "controller.h"
#include "switch.h"
#include "init.h"

int main(int argc, char* argv[]){

  // Error checking, for valid input
  if(check_argv_number(argc) == -1){ printf("Invalid Number of Arguments\n"); return 0; }
  if(check_prgm(argv[1]) == 1){
    if(check_cont_num(argv[2]) == -1){printf("Invalid Number of Controller\n");}
    // Execute Controller upon user request
    else{ controller(check_cont_num(argv[2])); }
  }else if(check_prgm(argv[1]) == 2){
    // Error check for valid number of switch (between 1 and 7)
    int ret_swi  = check_sw_num(argv[1]);
    int ret_swj  = check_sw_num(argv[3]);
    int ret_swk  = check_sw_num(argv[4]);

    if (ret_swi == -1 || ret_swj == -1 || ret_swk == -1) { printf("\nSWITCH OUT OF RANGE (MAX: 7 switches)\n"); return 0; }

    // Execute Switch upon user request
    switch_sdn(argv[1], argv[2], argv[3], argv[4], argv[5]);

  }else{ printf("Neither Controller (cont) or Switch (sw) Executed\n"); }
  return 0;
}
