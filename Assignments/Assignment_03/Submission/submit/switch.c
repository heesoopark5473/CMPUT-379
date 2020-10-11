#include "init.h"
#include "switch_functions.h"


int switch_sdn(char* swi, char* trafficFile, char* swj, char* swk, char* IP_range, char* serverAddress, char* portNumber){

    int sockfd = socket_connect(serverAddress, portNumber);

    FILE* file;
    char  file_array    [1024][1024];
    char  line_read[1024];
    int   line_count = 0;

    if(file_exist(trafficFile)){file = fopen(trafficFile, "r");}
    else{printf("\ntrafficFile Does Not Exist\n\n"); return 0;}

    while(fgets(line_read, sizeof(line_read), file) != NULL) {
        if(strncmp(line_read, " ", 1) != 0 && strncmp(line_read, "#", 1) != 0){strcpy(file_array[line_count], line_read); line_count++;}
    }

    ft_index = 0;
    int IPlow       = atoi(strtok(IP_range, "-"));
    int IPhigh      = atoi(strtok(NULL, " "));
    update_flowtable(0, MAXIP, IPlow, IPhigh, "FORWARD", 3, 4, 0);

    int ret_open = send_open_pkt(sockfd, swi, swj, swk, IP_range);
    if(ret_open > 0){
        ////////////////
        swi_data.open++;
        ////////////////
        printf("\n\tTransmitted (src= %s, dest = cont) [OPEN]:\n\t\t(port0= cont, port1= %s, port2= %s, port3= %s)\n", swi, swj, swk, IP_range);
    }else{printf("\nOPEN PACKET NOT SENT\n\n");}


    struct pollfd fds_sw[2];
    fds_sw[0].fd        = sockfd;
    fds_sw[0].events    = POLLIN;
    fds_sw[1].fd        = STDIN_FILENO;
    fds_sw[1].events    = POLLIN;

    int iter_count = 0;
    delay_flag = 0;

    while(1){

        int ret = poll(fds_sw, 2, TIMEOUT);

        if(ret == -1){perror("poll");}
        if(!ret){}

        if(iter_count < line_count+1 && swi_data.ack == 1){
          if(delay_flag == 0){
            if(strncmp(file_array[iter_count], swi, 3) == 0){
              char ret_pkt[1024] = {0};
              calculate_pkt(ret_pkt, file_array[iter_count]);
            }
          }else if(delay_flag == -1){
              continue;
          }
          iter_count++;
        }

        if(fds_sw[0].revents & POLLIN){
            char buffer[1024] = {0};
            int valread = recv(sockfd, buffer, 1024, 0);

            if(valread < 0){printf("\n Read error \n");}
            else{
                if(strncmp(buffer, "ACK", 3) == 0){
                    ///////////////
                    swi_data.ack++;
                    ///////////////
                    char* type = strtok(buffer, " ");       UNUSED(type);
                    char* swi  = strtok(NULL, " ");

                    printf("\n\tReceived (src= cont, dest= %s) [ACK]\n\n", swi);

                    // for(int k=0; k<line_count+1; k++){
                    //   char ret_pkt[1024] = {0};
                    //   if(strncmp(file_array[k], swi, 3) == 0){
                    //     calculate_pkt(ret_pkt, file_array[k]);
                    //   }
                    // }
                }
            }
        }
        else if(fds_sw[1].revents & POLLIN){
          print_user_command();
        }
    }

    return 0;
}
