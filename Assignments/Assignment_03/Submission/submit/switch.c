#include "init.h"
#include "switch_functions.h"
#include "controller_functions.h"

int switch_sdn(char* swi, char* trafficFile, char* swj, char* swk, char* IP_range, char* serverAddress, char* portNumber){

    char swi_copy[4];
    strcpy(swi_copy, swi);
    int swi_int = atoi(strtok(swi_copy, "sw"));

    int cur_switch      = swi_int;
    int left_switch     = swi_int-1;
    int right_switch    = swi_int+1;

    char* read_left_fifo        = malloc(sizeof(char)*24);
    char* read_right_fifo       = malloc(sizeof(char)*24);
    char* write_left_fifo       = malloc(sizeof(char)*24);
    char* write_right_fifo      = malloc(sizeof(char)*24);
    char read_fifo_left_char[124];
    char read_fifo_right_char[124];
    char write_fifo_left_char[124];
    char write_fifo_right_char[124];

    fifo_name(read_fifo_left_char, left_switch, cur_switch);
    fifo_name(read_fifo_right_char, right_switch, cur_switch);
    fifo_name(write_fifo_left_char, cur_switch, left_switch);
    fifo_name(write_fifo_right_char, cur_switch, right_switch);

    strcpy(read_left_fifo, read_fifo_left_char);
    strcpy(read_right_fifo, read_fifo_right_char);
    strcpy(write_left_fifo, write_fifo_left_char);
    strcpy(write_right_fifo, write_fifo_right_char);

    mkfifo(read_left_fifo,    0666);
    mkfifo(read_right_fifo,   0666);
    mkfifo(write_left_fifo,   0666);
    mkfifo(write_right_fifo,  0666);

    printf("\n\n");

    int sockfd;

    struct hostent *hstnm;
    hstnm = gethostbyname(serverAddress);
    if(hstnm == NULL){
      printf("\ngethostbyname() failed\n\n");
    }else{
      //printf("%s = ", hstnm->h_name);
      unsigned int i=0;
      while(hstnm -> h_addr_list[i] != NULL){
        //printf("\n%s \n", inet_ntoa(*(struct in_addr*)(hstnm->h_addr_list[i])));
        sockfd = socket_connect(inet_ntoa(*(struct in_addr*)(hstnm->h_addr_list[i])), portNumber);
        if(sockfd == -1){
          return 1;
        }
        i++;
      }
    }

    int fd_read_left    = open(read_left_fifo,    O_RDONLY | O_NONBLOCK);
    int fd_read_right   = open(read_right_fifo,   O_RDONLY | O_NONBLOCK);

    FILE*   file;
    char    file_array[1024][1024];
    char    line_read[1024];
    int     line_count  = 0;
    //int     sockfd      = socket_connect(serverAddress, portNumber);


    if(file_exist(trafficFile)){file = fopen(trafficFile, "r");}
    else{printf("\ntrafficFile Does Not Exist\n\n"); return 0;}

    while(fgets(line_read, sizeof(line_read), file) != NULL) {
        if(strncmp(line_read, " ", 1) != 0 && strncmp(line_read, "#", 1) != 0){strcpy(file_array[line_count], line_read); line_count++;}}

    ft_index          = 0;

    char* IPlow_char  = strtok(IP_range,"-");
    char* IPhigh_char = strtok(NULL, "-");

    int IPlow         = atoi(IPlow_char);
    int IPhigh        = atoi(IPhigh_char);

    update_flowtable(0, MAXIP, IPlow, IPhigh, "FORWARD", 3, 4, 0);

    int ret_open      = send_open_pkt(sockfd, swi, swj, swk, IPlow_char, IPhigh_char);

    if(ret_open > 0){
        ////////////////
        swi_data.open++;
        ////////////////
        printf("\tTransmitted (src= %s, dest = cont) [OPEN]:\n\t\t(port0= cont, port1= %s, port2= %s, port3= %s-%s)\n", swi, swj, swk, IPlow_char, IPhigh_char);
    }else{printf("\nOPEN PACKET NOT SENT\n\n");}


    struct pollfd fds_sw[4];

    fds_sw[0].fd        = sockfd;
    fds_sw[0].events    = POLLIN;

    fds_sw[1].fd        = fd_read_left;
    fds_sw[1].events    = POLLIN;

    fds_sw[2].fd        = fd_read_right;
    fds_sw[2].events    = POLLIN;

    fds_sw[3].fd        = STDIN_FILENO;
    fds_sw[3].events    = POLLIN;

    int iter_count      = 0;
    delay_flag          = 0;

    int   my_flag = 0;

    while(1){
        int ret         = poll(fds_sw, 4, TIMEOUT);

        if(ret == -1){perror("poll");}
        if(!ret){}

        if(iter_count < line_count+1 && my_flag == 0){
            char ret_pkt[1024] = {0};
            if(delay_flag == 0){
                if(strncmp(file_array[iter_count], swi, 3) == 0){
                  calculate_pkt(ret_pkt, file_array[iter_count]);
                  if(strncmp(ret_pkt, "ADD", 3) == 0){
                  }else if(strncmp(ret_pkt, "QUERY", 5) == 0){
                    /////////////////
                    swi_data.query++;
                    /////////////////
                    write(sockfd, ret_pkt, strlen(ret_pkt));
                    my_flag = 1;
                  }else if(strncmp(ret_pkt, "RELAY", 5) == 0){
                    char copy_ret[32];
                    strcpy(copy_ret, ret_pkt);
                    char* type = strtok(copy_ret, " ");     UNUSED(type);
                    char* des_sw = strtok(NULL, " ");
                    char* srcIP  = strtok(NULL, " ");       UNUSED(srcIP);
                    char* desIP  = strtok(NULL, " ");       UNUSED(desIP);
                    char* IPlo   = strtok(NULL, " ");
                    char* IPhi   = strtok(NULL, " ");

                    int lo_len = strlen(IPlo);
                    if( IPlo[lo_len-1] == '\n' )
                        IPlo[lo_len-1] = 0;

                    int hi_len = strlen(IPhi);
                    if( IPhi[hi_len-1] == '\n' )
                        IPhi[hi_len-1] = 0;

                    printf("\n\tTransmitted (src= %s, dest= sw%s) [RELAY]:   header= (srcIP= %s, destIP= %s)\n", swi, des_sw, IPlo, IPhi);

                    int destination = atoi(des_sw);
                    int current     = swi_int;
                    if(destination > current){
                      swi_data.relayout++;
                    }else if(destination < current){
                      swi_data.relayout++;
                    }
                    if(destination < current){
                      int fd_write_left   = open(write_left_fifo, O_WRONLY);
                      write(fd_write_left, ret_pkt, strlen(ret_pkt));
                      close(fd_write_left);
                    }else if(destination > current){
                      int fd_write_right  = open(write_right_fifo, O_WRONLY);
                      write(fd_write_right, ret_pkt, strlen(ret_pkt));
                      close(fd_write_right);
                    }
                  }
                }
              iter_count++;
            }
        }

        if(fds_sw[0].revents & POLLIN){
            char buffer[254] = {0};

            recv(sockfd, buffer, sizeof(buffer), 0);

            if(strncmp(buffer, "ACK", 3) == 0){
                my_flag = 0;

                ///////////////
                swi_data.ack++;
                ///////////////
                char* type      = strtok(buffer, " ");       UNUSED(type);
                char* des_swi   = strtok(NULL, " ");

                printf("\tReceived (src= cont, dest= %s) [ACK]\n\n", des_swi);
            }
            if(strncmp(buffer, "ADDRULE", 7) == 0){
              my_flag = 0;
              ///////////////////
              swi_data.addrule++;
              ///////////////////
              char* type  = strtok(buffer, " ");        UNUSED(type);
              char* srcIP = strtok(NULL, " ");          UNUSED(srcIP);
              char* desIP = strtok(NULL, " ");
              printf("\tReceived (src= cont, dest= %s) [ADD]:\n\t\t(srcIP= %d-%d, destIP= %s-%s, action= DROP: 0, pri= 4, pktCount= 0)\n\n", swi, SRCIP_LO, SRCIP_HI, desIP, desIP);

              ft_index++;

              update_flowtable(SRCIP_LO, SRCIP_HI, atoi(desIP), atoi(desIP), "DROP", 0, 4, ft_struct[ft_index].pktCount+1);
            }

            if(strncmp(buffer, "RELAY", 5) == 0){
              char relay_message[124];
              strcpy(relay_message, buffer);
              my_flag = 0;
              ///////////////////
              swi_data.addrule++;
              ///////////////////
              char* type      = strtok(buffer, " ");        UNUSED(type);
              char* des_swi   = strtok(NULL, " ");
              char* srcIP     = strtok(NULL, " ");
              char* desIP     = strtok(NULL, " ");
              char* IPlo      = strtok(NULL, " ");
              char* IPhi      = strtok(NULL, " ");

              int lo_len = strlen(IPlo);
              if( IPlo[lo_len-1] == '\n' )
                  IPlo[lo_len-1] = 0;

              int hi_len = strlen(IPhi);
              if( IPhi[hi_len-1] == '\n' )
                  IPhi[hi_len-1] = 0;


              printf("\tReceived (src= cont, dest= %s) [ADD]:\n\t\t(srcIP= %d-%d, destIP= %s-%s, action= FORWARD: %s, pri= 4, pktCount= 0)\n\n", swi, SRCIP_LO, SRCIP_HI, srcIP, desIP, des_swi);

              ft_index++;

              update_flowtable(SRCIP_LO, SRCIP_HI, atoi(srcIP), atoi(desIP), "FORWARD", atoi(des_swi), 4, ft_struct[ft_index].pktCount+1);


              int destination = atoi(des_swi);
              int current     = atoi(strtok(swi, "sw"));

              if(destination > current){
                printf("\n\tTransmitted (src= sw%d, dest= sw%d) [RELAY]:   header= (srcIP= %s, destIP= %s)\n", current, destination, IPlo, IPhi);
                swi_data.relayout++;

              }else if(destination < current){
                printf("\n\tTransmitted (src= sw%d, dest= sw%d) [RELAY]:   header= (srcIP= %s, destIP= %s)\n", current, destination, IPlo, IPhi);
                swi_data.relayout++;
              }

              if(destination < current){
                int fd_write_left   = open(write_left_fifo, O_WRONLY);
                write(fd_write_left, relay_message, strlen(relay_message));
                close(fd_write_left);
              }else if(destination > current){
                int fd_write_right  = open(write_right_fifo, O_WRONLY);
                write(fd_write_right, relay_message, strlen(relay_message));
                close(fd_write_right);
              }
          }
        }

        if(fds_sw[1].revents & POLLIN){
          char left_message[254] = {0};
          char copy_message[254];
          read(fd_read_left, left_message, sizeof(char)*254);
          strcpy(copy_message, left_message);

          if(strncmp(copy_message, "RELAY", 5) == 0){
            char* type    = strtok(copy_message, " ");  UNUSED(type);
            char* des_sw  = strtok(NULL, " ");
            char* srcIP   = strtok(NULL, " ");      UNUSED(srcIP);
            char* desIP   = strtok(NULL, " ");
            char* IPlo    = strtok(NULL, " ");
            char* IPhi    = strtok(NULL, " ");
            char* src_sw  = strtok(NULL, " ");

            int lo_len = strlen(IPlo);
            if( IPlo[lo_len-1] == '\n' )
                IPlo[lo_len-1] = 0;

            int hi_len = strlen(IPhi);
            if( IPhi[hi_len-1] == '\n' )
                IPhi[hi_len-1] = 0;

            if(atoi(des_sw) == swi_int){
              printf("\tReceived (src= %s, dest= sw%s) [RELAY]: header= (srcIP= %s, desIP= %s)\n", src_sw, des_sw, IPlo, IPhi);
              swi_data.relayin++;
              for(int x=0; x<ft_index+1; x++){
                if(atoi(desIP) >= ft_struct[x].desIP_lo && atoi(desIP) <= ft_struct[x].desIP_hi){
                  ft_struct[x].pktCount++;
                }
              }
            }else if(atoi(des_sw) > swi_int){
              printf("\n\tTransmitted (src= sw%d, dest= sw%s) [RELAY]:   header= (srcIP= %s, destIP= %s)\n", swi_int, des_sw, IPlo, IPhi);
              swi_data.relayout++;
              int fd_write_right  = open(write_right_fifo, O_WRONLY);
              write(fd_write_right, left_message, strlen(left_message));
              close(fd_write_right);
            }else if(atoi(des_sw) < swi_int){
              printf("\n\tTransmitted (src= sw%d, dest= sw%s) [RELAY]:   header= (srcIP= %s, destIP= %s)\n", swi_int, des_sw, IPlo, IPhi);
              swi_data.relayout++;
              int fd_write_left   = open(write_left_fifo, O_WRONLY);
              write(fd_write_left, left_message, strlen(left_message));
              close(fd_write_left);
            }
          }
        }

        if(fds_sw[2].revents & POLLIN){
          char right_message[254] = {0};
          char copy_message[254];
          read(fd_read_right, right_message, sizeof(char)*254);
          strcpy(copy_message, right_message);

          if(strncmp(copy_message, "RELAY", 5) == 0){

            char* type    = strtok(copy_message, " ");        UNUSED(type);
            char* des_sw  = strtok(NULL, " ");
            char* srcIP   = strtok(NULL, " ");                UNUSED(srcIP);
            char* desIP   = strtok(NULL, " ");
            char* IPlo    = strtok(NULL, " ");
            char* IPhi    = strtok(NULL, " ");
            char* src_sw  = strtok(NULL, " ");

            int lo_len = strlen(IPlo);
            if( IPlo[lo_len-1] == '\n' )
                IPlo[lo_len-1] = 0;

            int hi_len = strlen(IPhi);
            if( IPhi[hi_len-1] == '\n' )
                IPhi[hi_len-1] = 0;

            if(atoi(des_sw) == swi_int){
              printf("\tReceived (src= %s, dest= sw%s) [RELAY]: header= (srcIP= %s, desIP= %s)\n", src_sw, des_sw, IPlo, IPhi);
              swi_data.relayin++;
              for(int x=0; x<ft_index+1; x++){
                if(atoi(desIP) >= ft_struct[x].desIP_lo && atoi(desIP) <= ft_struct[x].desIP_hi){
                  ft_struct[x].pktCount++;
                }
              }
            }else if(atoi(des_sw) > swi_int){
              printf("\n\tTransmitted (src= sw%d, dest= sw%s) [RELAY]:   header= (srcIP= %s, destIP= %s)\n", swi_int, des_sw, IPlo, IPhi);
              swi_data.relayout++;
              int fd_write_right  = open(write_right_fifo, O_WRONLY);
              write(fd_write_right, right_message, strlen(right_message));
              close(fd_write_right);
            }else if(atoi(des_sw) < swi_int){
              printf("\n\tTransmitted (src= sw%d, dest= sw%s) [RELAY]:   header= (srcIP= %s, destIP= %s)\n", swi_int, des_sw, IPlo, IPhi);
              swi_data.relayout++;
              int fd_write_left   = open(write_left_fifo, O_WRONLY);
              write(fd_write_left, right_message, strlen(right_message));
              close(fd_write_left);
            }
          }
        }
        if(fds_sw[3].revents & POLLIN){
          print_user_command_switch();
        }
    }
    return 0;
}
