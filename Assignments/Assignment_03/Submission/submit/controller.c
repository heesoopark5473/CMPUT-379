#include "init.h"
#include "controller_functions.h"

int controller_sdn(int nSwitch, char* portNumber){

    print_welcome_cont();

    // create_fifo_switches(nSwitch);

    pthread_t thread1;
    pthread_create(&thread1, NULL, socket_accept, (void*) portNumber);

    fds_cont[7].fd       = STDIN_FILENO;
    fds_cont[7].events   = POLLIN;

    sw_index = 0;

    while(1){

        char message[254] = {0};

        int ret         = poll(fds_cont, 8, TIMEOUT);

        if(ret == -1){perror("poll");}
        if(!ret){}

        for(int i=0; i<MAX_NSW; i++){

            if(fds_cont[i].revents & POLLIN){

                int ret_read = read(fds_cont[i].fd, message, sizeof(message));

                char copy_message[254];
                strcpy(copy_message, message);
                //printf("%s\n", message);
                if(ret_read == 0){
                    printf("\n\nlost connection to sw%d\n\n", i+1);
                    close(fds_cont[i].fd);
                }

                if(strncmp(message, "OPEN", 4) == 0){
                    /////////////////
                    cont_data.open++;
                    /////////////////
                    char* type      = strtok(copy_message, " ");                        UNUSED(type);
                    char* swi       = strtok(NULL, " ");
                    char* swj       = strtok(NULL, " ");
                    char* swk       = strtok(NULL, " ");
                    char* IPlow     = strtok(NULL, " ");
                    char* IPhigh    = strtok(NULL, " ");


                    printf("\tReceived (src= %s, dest= cont) [OPEN]:\n\t\t(port0= cont, port1= %s, port2= %s, port3= %s-%s)\n", swi, swj, swk, IPlow, IPhigh);

                    char* swi_token     = strtok(swi, "sw");
                    char* swj_token     = strtok(swj, "sw");
                    char* swk_token     = strtok(swk, "sw");

                    sw_index++;

                    store_switch_info(sw_index, swi_token, swj_token, swk_token, IPlow, IPhigh);

                    int ret_ack = send_ack_pkt(fds_cont[i].fd, swi);
                    if(ret_ack > 0){
                    ////////////////
                    cont_data.ack++;
                    ////////////////
                      printf("\tTransmitted (src= cont, dest= sw%s) [ACK]\n", swi_token);
                    }else{printf("\n ACK PACKET NOT SENT\n\n");}
                }

                if(strncmp(message, "QUERY", 5) == 0){
                    //////////////////
                    cont_data.query++;
                    //////////////////
                    char* type      = strtok(copy_message, " ");                UNUSED(type);
                    char* swi       = strtok(NULL, " ");
                    char* srcIP     = strtok(NULL, " ");
                    char* desIP     = strtok(NULL, " ");

                    int src_len = strlen(srcIP);
                    if( srcIP[src_len-1] == '\n' )
                        srcIP[src_len-1] = 0;
                    int des_len = strlen(desIP);
                    if( desIP[des_len-1] == '\n' )
                        desIP[des_len-1] = 0;
                    printf("\tReceived (src= %s, dest= cont) [QUERY]: header= (srcIP= %s, desIP= %s)\n", swi, srcIP, desIP);

                    int flag = 0;
                    int relay_sw;
                    char ret_swi[16];

                    for(int j=1; j<MAX_NSW+1; j++){
                        if(atoi(desIP) >= atoi(sw_struct[j].srcIP) && atoi(desIP) <= atoi(sw_struct[j].desIP)){
                          flag = 1;
                          relay_sw = j;
                          strcpy(ret_swi, sw_struct[j].swi);
                        }
                    }
                    if(flag == 0){
                      printf("\tTransmitted (src= cont, dest= %s) [ADD]:\n\t\t(srcIP= %d-%d, desIP= %s-%s, action= DROP:0, pri= 4, pktCount= 0)\n", swi, SRCIP_LO, SRCIP_HI, desIP, desIP);
                      cont_data.add++;
                      send_add_pkt(fds_cont[i].fd, srcIP, desIP);
                    }else if(flag == 1){
                      printf("\tTransmitted (src= cont, dest= %s) [ADD]:\n\t\t(srcIP= %d-%d, desIP= %s-%s, action= FORWARD: %d, pri= 4, pktCount= 0)\n", swi, SRCIP_LO, SRCIP_HI, sw_struct[relay_sw].srcIP, sw_struct[relay_sw].desIP, relay_sw);
                      cont_data.add++;
                      send_relay_pkt(fds_cont[i].fd, sw_struct[relay_sw].swi, sw_struct[relay_sw].srcIP, sw_struct[relay_sw].desIP , srcIP, desIP, swi);
                    }
                }
            }
            fds_cont[i].fd       = fds_array[i][0];
            fds_cont[i].events   = fds_array[i][1];
        }
        if(fds_cont[7].revents & POLLIN){
          print_user_commnad_cont(nSwitch);
        }
    }
    return 0;
}
