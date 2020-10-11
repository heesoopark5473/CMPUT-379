#include "init.h"
#include "controller_functions.h"

int controller_sdn(int nSwitch, char* portNumber){

    print_welcome_cont();

    create_fifo_switches(nSwitch);

    pthread_t thread1;
    int iret1 = pthread_create(&thread1, NULL, socket_accept, (void*) portNumber);

    fds_cont[7].fd       = STDIN_FILENO;
    fds_cont[7].events   = POLLIN;

    while(1){

        char message[1024] = {0};

        int ret         = poll(fds_cont, 8, TIMEOUT);

        if(ret == -1){perror("poll");}
        if(!ret){}

        for(int i=0; i<MAX_NSW+1; i++){

            if(fds_cont[i].revents & POLLIN){
                int ret_read = recv(fds_cont[i].fd, message, 1024, 0);
                printf("%s\n", message);
                if(ret_read == 0){
                    printf("SWITCH CLOSED\n");
                    close(fds_cont[i].fd);
                }

                if(strncmp(message, "OPEN", 4) == 0){
                    /////////////////
                    cont_data.open++;
                    /////////////////

                    char copy_message[1024];
                    strcpy(copy_message, message);

                    char* type      = strtok(copy_message, " ");                        UNUSED(type);
                    char* swi       = strtok(NULL, " ");
                    char* swj       = strtok(NULL, " ");
                    char* swk       = strtok(NULL, " ");
                    char* IP_range  = strtok(NULL, " ");

                    printf("\n\tReceived (src= %s, dest= cont) [OPEN]:\n\t\t(port0= cont, port1= %s, port2= %s, port3= %s)\n", swi, swj, swk, IP_range);

                    char* swi_token     = strtok(swi, "sw");
                    char* swj_token     = strtok(swj, "sw");
                    char* swk_token     = strtok(swk, "sw");
                    char* srcIP_token   = strtok(IP_range, "-");
                    char* desIP_token   = strtok(NULL, " ");
                    int   index         = atoi(swi_token);

                    store_switch_info(index, swi_token, swj_token, swk_token, srcIP_token, desIP_token);

                    int ret_ack = send_ack_pkt(fds_cont[i].fd, swi);
                    if(ret_ack > 0){
                      ////////////////
                      cont_data.ack++;
                      ////////////////
                      printf("\n\tTransmitted(src= cont, dest= sw%s) [ACK]\n\n", swi_token);
                    }else{printf("\n ACK PACKET NOT SENT\n\n");}

                }
            }
            fds_cont[i].fd       = fds_array[i][0];
            fds_cont[i].events   = fds_array[i][1];
        }
    }

    // while(1){
    //     char buffer[1024] = {0};
    //     connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
    //     read(connfd, buffer, 1024);
    //     printf("%s\n", buffer);
    //     write(connfd, "ACK Packet", strlen("ACK Packet"));
    //     printf("ACK Packet Requested\n");
    //     close(connfd);
    //     sleep(1);
    //  }
    return 0;
}
