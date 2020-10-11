//Include necessary header files
#include "init.h"
#include "controller_function.h"
#include "switch_function.h"

int controller(int nSwitch){

  printf("\n");
  printf("  ##########################################################################\n");
  printf("  #                                                                        #\n");
  printf("  #             CMPUT 379 -- a2sdn (Controller)                            #\n");
  printf("  #                                                                        #\n");
  printf("  #             (1) Please Turn on Controller                              #\n");
  printf("  #             (2) Please Turn on Switches in increasing order            #\n");
  printf("  #             (3) Please wait around 3-6 seconds to turn on              #\n");
  printf("  #                 the next switch (depending on size of trafficFile)     #\n");
  printf("  #             (4) Type 'list' to view the status                         #\n");
  printf("  #             (5) Type 'quit' to view the status and terminate           #\n");
  printf("  #                                                                        #\n");
  printf("  #                                                                        #\n");
  printf("  #              User Command List:                                        #\n");
  printf("  #             (1) list - View the status of Contorller or Switch         #\n");                        
  printf("  #             (2) exit - View teh status and terminate                   #\n");
  printf("  #                                                                        #\n");
  printf("  #                                                                        #\n");
  printf("  #                 Thank you -by Hee Soo Park                             #\n");
  printf("  #                                                                        #\n");
  printf("  ##########################################################################\n");

  printf("\n\n");
  printf("  ##########################################################################\n");
  printf("  #                                                                        #\n");
  printf("  #                           CONTROLLER                                   #\n");
  printf("  #                                                                        #\n");
  printf("  ##########################################################################\n");

  printf("\n\n");

  char  fifo_array[nSwitch][2][32];
  char* swi_cont_fifo;
  char* cont_swi_fifo;
  int i;
  int k;

  // create fifo files for between switches for I/O multiplex communication

  create_switch_fifo(nSwitch);

  // create fifo files for connection between current switch and controller
  for(i=0; i<nSwitch; i++){
    swi_cont_fifo = fifo_name(i+1,0);
    cont_swi_fifo = fifo_name(0,i+1);
    mkfifo(swi_cont_fifo, 0666);
    mkfifo(cont_swi_fifo, 0666);
    strcpy(fifo_array[i][0], swi_cont_fifo);
    strcpy(fifo_array[i][1], cont_swi_fifo);
  }
  free(swi_cont_fifo);
  free(cont_swi_fifo);

  while(1){
    // loop for number of switches to poll for input from every switch that is executed
    for(k=0; k<nSwitch; k++){

      struct  pollfd  fds_cont[2];
      char*   message_cont;
      char*   copy_message_cont;
      char*   swi;
      int     int_swi;
      int     ret_cont;
      int     fd_read;

      fd_read = open(fifo_array[k][0], O_RDONLY | O_NONBLOCK);

      // Poll for incoming data, and user STDIN

      //READ
      fds_cont[0].fd     = fd_read;
      fds_cont[0].events = POLLIN;
      //STDIN
      fds_cont[1].fd     = STDIN_FILENO;
      fds_cont[1].events = POLLIN;

      ret_cont = poll(fds_cont, 2, TIMEOUT);

      if(ret_cont == -1){perror("poll");}
      if(!ret_cont){}

      if(fds_cont[0].revents & POLLIN){
          message_cont       = malloc(sizeof(char)*124);
          copy_message_cont  = malloc(sizeof(char)*124);

          read(fd_read, message_cont, sizeof(char)*124);

          // if received OPEN packet, return ACK pkt

          if(strncmp(message_cont, "OPEN", 4) == 0){
            swi = store_swi_info(message_cont);
            int_swi = get_swi_int(swi);
            // increment open packet count
            cont_data.open++;
            char ack_pkt[12];
            strcpy(ack_pkt, "ACK");
            strcat(ack_pkt, " ");
            strcat(ack_pkt, swi);
            int fd_open_write;
            fd_open_write = open(fifo_array[k][1], O_WRONLY);
            write(fd_open_write, ack_pkt, strlen(ack_pkt)+1);
            close(fd_open_write);
            cont_data.ack++;
          }

          int ret_swi;

          // if received QUERY packet look through the IP_range of executed to switch to 
          // transmitt back to the switch, where the packet belongs

          if(strncmp(message_cont, "QUERY", 5) == 0){
            cont_data.query++;
            strcpy(copy_message_cont, message_cont);
            char* string = strtok(copy_message_cont, " ");  UNUSED(string);
            char* src_sw = strtok(NULL, " ");
            char* srcip  = strtok(NULL, " ");               UNUSED(srcip);
            char* desip  = strtok(NULL, " ");

            int int_des = atoi(desip);
            int flag = 0;
            int copy_int;
            ret_swi = int_swi;

            // look thorough IP_range of currently executed switches
            for(int t=1; t<int_swi+1; t++){
              int low_ip  = get_ip_int(sw_struct[t].IP_range, 1);
              int high_ip = get_ip_int(sw_struct[t].IP_range, 2);
              if(int_des >= low_ip && int_des <= high_ip){
                flag = 1;
                copy_int = t;
                ret_swi = atoi(sw_struct[t].swi);
              }
            }

            // if the packet does not belong in any of the IP_range of executed switches,
            // then transmit ADDRULE packet

            if(flag == 0){
              // increment add packet count
              cont_data.add++;
              int fd_add_write;
              fd_add_write = open(fifo_array[k][1], O_WRONLY);
              char*   add_pkt;
              add_pkt = malloc(sizeof(char)*24);
              strcpy(add_pkt, "ADDRULE");
              strcat(add_pkt, " "); 
              strcat(add_pkt, src_sw);   strcat(add_pkt, " ");
              strcat(add_pkt, desip); strcat(add_pkt, " ");
              write(fd_add_write, add_pkt, strlen(add_pkt)+1);
              close(fd_add_write);
            }
            // if the packet does belong in one of the IP_range of executed switches,
            // then transmit RELAY packet
            else if(flag > 0){
              char str[2];
              // increment add packet count
              cont_data.add++;
              char* relay_pkt;
              relay_pkt = malloc(sizeof(char)*24);
              int fd_relay_write;
              fd_relay_write = open(fifo_array[k][1], O_WRONLY);
              sprintf(str, "%d", ret_swi);
              strcat(relay_pkt, "RELAY"); strcat(relay_pkt, " ");
              strcat(relay_pkt, desip);   strcat(relay_pkt, " ");
              strcat(relay_pkt, str);    strcat(relay_pkt, " ");
              strcat(relay_pkt, sw_struct[copy_int].IP_range); strcat(relay_pkt, " ");
              write(fd_relay_write, relay_pkt, strlen(relay_pkt)+1);
              close(fd_relay_write);
            }
          }
          free(message_cont);
          free(copy_message_cont);
      }
      if(fds_cont[1].revents & POLLIN){
        print_list_quit_cont(nSwitch);
      }
          close(fd_read);

    }
  }
  return 0;
}
