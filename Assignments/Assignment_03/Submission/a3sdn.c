#include "init.h"
#include "a3sdn_functions.h"
#include "controller.h"
#include "controller_functions.h"
#include "switch.h"
#include "switch_functions.h"

int main(int argc, char* argv[]){
    
    if(argc == 1){printf("\nPlease Execute Controller (cont) or Switch (sw)\n\n"); return 0;}

    if(check_prgm(argv[1]) == 1){
        if(check_argc(argv[1], argc) == 1){
            if(check_nSwitch(argv[2]) == -1){
                printf("\nSwitch Out of Range (MAX_NSW == 7)\n\n"); return 0;
            }else{
                controller_sdn(atoi(argv[2]), argv[3]);
            }
        }else{
            printf("\nInvalid Number of Controller Arguments (e.g., a3sdn cont nSwitch portNumber)\n\n"); return 0;
        }
    
    }else if(check_prgm(argv[1]) == 2){
        if(check_argc(argv[1], argc) == 1){
            if(check_sw(argv[1]) == -1 || check_sw(argv[3]) == -1 || check_sw(argv[4]) == -1){
                printf("\nSwich Out of Range (MAX NSW == 7)\n\n"); return 0;
            }else{
                switch_sdn(argv[1], argv[2], argv[3], argv[4], argv[5], argv[6], argv[7]);
            }
        }else{
            printf("\nInvalid Number of Switch Arguments (e.g., swi trafficFile (null|swj) (null|swk) IPlow_IPhigh serverAddress PortNumber)\n\n");
        }
    }else{printf("\nNeither Controller (cont) or Switch (sw) Executed\n\n");}

    
    return 0;
}