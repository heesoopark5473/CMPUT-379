#include "init.h"

int check_prgm(char* prgm){
    if      (strncmp(prgm, "cont", 4) == 0){return 1;}
    else if (strncmp(prgm, "sw",   2) == 0){return 2;}
    else    {return -1;}
}

int check_argc(char* prgm, int argc){
    if(strncmp(prgm, "cont", 4) == 0){
        if(argc == 4){return 1;}
        else{return -1;}
    }else if(strncmp(prgm, "sw", 2) == 0){
        if(argc == 8){return 1;}
        else{return -1;}
    }else{return -1;}
}

int check_nSwitch(char* nSwitch){
    if(atoi(nSwitch) > MAX_NSW || atoi(nSwitch) < 1){return -1;}
    else{return atoi(nSwitch);}
}

int check_sw(char* sw){
    int sw_int;
    if(strncmp(sw, "sw", 2) == 0 && strlen(sw) > 2){
        sw_int = atoi(strtok(sw, "sw"));
        if(sw_int > MAX_NSW || sw_int < 1){return -1;}
        else{return sw_int;}
    }
    else if(strncmp(sw, "null", 4) == 0){sw_int = atoi(sw); return sw_int;}
    else{return -1;}
}