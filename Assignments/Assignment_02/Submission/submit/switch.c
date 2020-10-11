#include "init.h"
#include "switch_function.h"
#include "controller_function.h"

int switch_sdn(char* swi, char* trafficFile, char* swj, char* swk, char* IP_range){

  printf("\n\n");
  printf("  ##########################################################################\n");
  printf("  #                                                                        #\n");
  printf("  #                           SWITCH   %s                                 #\n",swi);
  printf("  #                                                                        #\n");
  printf("  ##########################################################################\n");

  printf("\n\tYou may Enter Commands (list/exit) at any time\n");

  printf("\n\tPlease Wait For 3-7 seconds to Execute Next Switch");

  FILE* file;
  char  file_array[100][100];
  int i=0;

  // check if the trafficFile exists
  if(file_exist(trafficFile)){file=fopen(trafficFile,"r");}
  else{printf("trafficFile Does Not Exist\n"); return 0;}

  // if exists, read the trafficFile line by line and store into an array
  while(fgets(file_array[i], sizeof(file_array[i]), file)){ i++; }

  // get the fifo name between current switch and controller
  char* swi_cont_fifo = fifo_name(get_swi_int(swi), 0);
  char* cont_swi_fifo = fifo_name(0, get_swi_int(swi));

  printf("\n\n");
  printf("\tSwitch Reads From Controller   : %s\n", cont_swi_fifo);
  printf("\tSwitch Writes To Controller    : %s\n\n", swi_cont_fifo);

  printf("\tREAD FROM LEFT                 : %s\n", fifo_name(get_swi_int(swi)-1, get_swi_int(swi)));
  printf("\tREAD FROM RIGHT                : %s\n", fifo_name(get_swi_int(swi)+1, get_swi_int(swi)));

  printf("\tWRITING TO LEFT SWITCH         : %s\n", fifo_name(get_swi_int(swi), get_swi_int(swi)-1));
  printf("\tWRITING TO RIGHT SWITHC        : %s\n", fifo_name(get_swi_int(swi), get_swi_int(swi)+1));

  printf("\n\n\n");


  int iter_count=0;
  flow_table_index=0;

  // intialize the flowtabl with given data on assignment page
  initialize_flowtable(IP_range);

  // create and open pkt to send to controller
  char* open_pkt  = get_open_pkt(swi,swj,swk,IP_range);

  // if connection has been made between current switch and controller, send an open packet
  if(file_exist(cont_swi_fifo) && file_exist(swi_cont_fifo)){
    swi_data.open = 1;
    int fd_write_open;
    fd_write_open = open(swi_cont_fifo, O_WRONLY);
    usleep(10000);
    write(fd_write_open, open_pkt, strlen(open_pkt)+1);
    close(fd_write_open);
    free(open_pkt);
  }else{printf("\nPORT 0 NOT CONNECTED --  FIFO HAVE NOT BEEN CREAED\n");}



  while(1){

    // For every line in traffic File, calculate what switch should do with every line
    if(iter_count < i+1){
        // calculate the packet
        char* query_pkt = calculate_pkt(file_array[iter_count-1], swi, get_ip_int(IP_range,1), get_ip_int(IP_range,2));
        // if switch should query the packet, then send to controller
        if(strncmp(query_pkt, "QUERY", 5) == 0){
          // increment query count
          swi_data.query++;
          int fd_write_query;
          fd_write_query = open(swi_cont_fifo, O_WRONLY);
          usleep(10000);
          write(fd_write_query, query_pkt, strlen(query_pkt)+1); 
          close(fd_write_query);
          free(query_pkt);
        }else if(strncmp(query_pkt, "ADD", 3) == 0){
        // if switch should relay the packet, then send to neighbor switch, correspond to output of calculated packet
        }else if(strncmp(query_pkt, "RELAY", 5) == 0){
          swi_data.relayout++;
          char* copy_query_pkt;
          copy_query_pkt = malloc(sizeof(char)*32);
          
          strcpy(copy_query_pkt, query_pkt);
          char* relay_string = strtok(copy_query_pkt, " "); UNUSED(relay_string);
          char* relay_ip = strtok(NULL, " ");
          char* relay_ad = strtok(NULL, " ");

          char* final_ret = malloc(sizeof(char)*32);
          strcpy(final_ret, "RELAY");
          strcat(final_ret, " ");
          strcat(final_ret, relay_ip);
          strcat(final_ret, " ");
          strcat(final_ret, relay_ad);
          // if destination switch is smaller then current switch, send to left neighbor
          if(atoi(relay_ad) < get_swi_int(swi)){
            int swi_relay_left;
            swi_relay_left = open(fifo_name(get_swi_int(swi), get_swi_int(swi)-1),  O_WRONLY);
            usleep(10000);
            write(swi_relay_left, final_ret, strlen(final_ret)+1);
            close(swi_relay_left);
          // if destination switch is greater than current switch, send to right neighbor
          }else if(atoi(relay_ad) > get_swi_int(swi)){
            int swi_relay_right;
            swi_relay_right = open(fifo_name(get_swi_int(swi), get_swi_int(swi)+1),  O_WRONLY);
            usleep(10000);
            write(swi_relay_right, final_ret, strlen(final_ret)+1);
            close(swi_relay_right);          
          }
          free(final_ret);
        }
    }
    
    // increment after each line is read
    iter_count++;

    struct  pollfd fds_swi[4];
    int     ret_swi;

    int fd_read_cont;
    int fd_read_left;
    int fd_read_right;

    char* message_swi;
    char* copy_message_swi;

    int cur_switch  = get_swi_int(swi);
    int left_switch = get_swi_int(swi)-1;
    int right_switch= get_swi_int(swi)+1;

    // open FIFO files to read from Controller, left neighbor switch, right neighbor switch
    fd_read_cont      = open(cont_swi_fifo, O_RDONLY | O_NONBLOCK);
    fd_read_left      = open(fifo_name(left_switch, cur_switch),  O_RDONLY | O_NONBLOCK);
    fd_read_right     = open(fifo_name(right_switch, cur_switch), O_RDONLY | O_NONBLOCK);

    // poll for incoming input from controller, left switch, right switch and user STDIN
    fds_swi[0].fd     = fd_read_cont;
    fds_swi[0].events = POLLIN;

    fds_swi[1].fd     = fd_read_left;
    fds_swi[1].events = POLLIN;

    fds_swi[2].fd     = fd_read_right;
    fds_swi[2].events = POLLIN;

    fds_swi[3].fd     = STDIN_FILENO;
    fds_swi[3].events = POLLIN;

    ret_swi           = poll(fds_swi, 4, TIMEOUT);

    if(ret_swi == -1){ perror("poll");}
    if(!ret_swi){}

    // polling for incoming input from controller
    if(fds_swi[0].revents & POLLIN){

      message_swi       = malloc(sizeof(char)*48);
      copy_message_swi  = malloc(sizeof(char)*48);

      read(fd_read_cont, message_swi, sizeof(char)*48);

      // if received ACK packet, then increment ack counter
      if(strncmp(message_swi, "ACK", 3) == 0){
        char* copy_ack;
        copy_ack = malloc(sizeof(char)*12);
        strcpy(copy_ack, message_swi);
        char* sw_1 = strtok(copy_ack, " "); UNUSED(sw_1);
        char* sw_2 = strtok(NULL, " ");     UNUSED(sw_2);
        swi_data.ack++;
      }

      // if received ADDRULE packet, then incrent addrule counter and add that rule to flow_table
      if(strncmp(message_swi, "ADDRULE", 7) == 0){
        strcpy(copy_message_swi, message_swi);
        char* t0 = strtok(copy_message_swi, " "); UNUSED(t0);
        char* t1 = strtok(NULL, " ");             UNUSED(t1);
        char* t2 = strtok(NULL, " ");
        char* t3 = strtok(NULL, " ");             UNUSED(t3);
        swi_data.addrule++;
        flow_table_index++;
        add_drop_rule(t2, flow_table_index);
      }

      // if received RELAY packet, add that rule to flow table and relay to corresponding switch
      if(strncmp(message_swi, "RELAY", 5) == 0){

        strcpy(copy_message_swi, message_swi);
        swi_data.addrule++;
        char* relay_str = strtok(copy_message_swi, " ");  UNUSED(relay_str);
        char* dest_ip   = strtok(NULL, " ");              UNUSED(dest_ip);
        char* dest_sw   = strtok(NULL, " ");     
        char* dest_range= strtok(NULL, " ");

        if(atoi(dest_sw) == get_swi_int(swi)){

        } else if(atoi(dest_sw) > get_swi_int(swi)){

          swi_data.relayout++;

          flow_table_index++;
          ft_struct[flow_table_index].srcIP_lo = SRCIP_LO;
          ft_struct[flow_table_index].srcIP_hi = SRCIP_HI;
          ft_struct[flow_table_index].desIP_lo = get_ip_int(dest_range,1);
          ft_struct[flow_table_index].desIP_hi = get_ip_int(dest_range,2);
          ft_struct[flow_table_index].actionType = "FORWARD";
          ft_struct[flow_table_index].actionVal = atoi(dest_sw);
          ft_struct[flow_table_index].pri    = 4;
          ft_struct[flow_table_index].pktCount++;
          
          int fd_write_right;
          fd_write_right = open(fifo_name(cur_switch, right_switch),  O_WRONLY);
          usleep(10000);
          write(fd_write_right, message_swi, strlen(message_swi)+1);
          close(fd_write_right);
                  
        }else if(atoi(dest_sw) < get_swi_int(swi)){
          
          swi_data.relayout++;

          flow_table_index++;
          ft_struct[flow_table_index].srcIP_lo = SRCIP_LO;
          ft_struct[flow_table_index].srcIP_hi = SRCIP_HI;
          ft_struct[flow_table_index].desIP_lo = get_ip_int(dest_range,1);
          ft_struct[flow_table_index].desIP_hi = get_ip_int(dest_range,2);
          ft_struct[flow_table_index].actionType = "FORWARD";
          ft_struct[flow_table_index].actionVal = atoi(dest_sw);
          ft_struct[flow_table_index].pri    = 4;
          ft_struct[flow_table_index].pktCount++;

          int fd_write_left;
          fd_write_left = open(fifo_name(cur_switch, left_switch),  O_WRONLY);
          usleep(10000);
          write(fd_write_left, message_swi, strlen(message_swi)+1);
          close(fd_write_left);
        }
      }
    }

    // polling for incoming input from left neighbor switch
    if(fds_swi[1].revents & POLLIN){
      char* message_left;
      char* left_copy;
      message_left       = malloc(sizeof(char)*48);
      left_copy          = malloc(sizeof(char)*48);

      read(fd_read_left, message_left, sizeof(char)*48);

      // if RELAY packet received, check if there is a rule that fits the packet
      if(strncmp(message_left, "RELAY", 5) == 0){
        
        strcpy(left_copy, message_left);
        
        char* relay_l    = strtok(left_copy, " ");  UNUSED(relay_l);
        char* left_ip    = strtok(NULL, " ");
        char* left_sw    = strtok(NULL, " ");
        char* left_range = strtok(NULL, " ");   UNUSED(left_range);

        for(int x=0; x<flow_table_index+1; x++){
          if(atoi(left_ip) >= ft_struct[x].desIP_lo && atoi(left_ip) <= ft_struct[x].desIP_hi){
            ft_struct[x].pktCount++;
          }
        }

        // if destination switch is not current switch keep relaying
        if(atoi(left_sw) == get_swi_int(swi)){
          swi_data.relayin++;
        } else if(atoi(left_sw) > get_swi_int(swi)){
            swi_data.relayout++;

            int fd_left_right;
            fd_left_right = open(fifo_name(cur_switch, left_switch),  O_WRONLY);
            usleep(10000);
            write(fd_left_right, message_left, strlen(message_left)+1);
            close(fd_left_right);
        } else if(atoi(left_sw) < get_swi_int(swi)){
            swi_data.relayout++;

            int fd_left_left;
            fd_left_left = open(fifo_name(cur_switch, left_switch),  O_WRONLY);
            usleep(10000);
            write(fd_left_left, message_left, strlen(message_left)+1);
            close(fd_left_left);          
        }
      }
    }

    // polling for input from right switch
    if(fds_swi[2].revents & POLLIN){
      char* message_right;
      char* right_copy;
      message_right       = malloc(sizeof(char)*48);
      right_copy          = malloc(sizeof(char)*48);

      read(fd_read_right, message_right, sizeof(char)*48);

      // if RELAY packet received, check if there is a rule that fits the packet
      if(strncmp(message_right, "RELAY", 5) == 0){
        
        strcpy(right_copy, message_right);
        
        char* relay       = strtok(right_copy, " ");  UNUSED(relay);
        char* right_ip    = strtok(NULL, " ");
        char* right_sw    = strtok(NULL, " ");
        char* right_range = strtok(NULL, " ");    UNUSED(right_range);

        for(int w=0; w<flow_table_index+1; w++){
          if(atoi(right_ip) >= ft_struct[w].desIP_lo && atoi(right_ip) <= ft_struct[w].desIP_hi){
            ft_struct[w].pktCount++;
          }
        }

        if(atoi(right_sw) == get_swi_int(swi)){
          swi_data.relayin++;
        
        // if destination switch is not current switch keep relaying
        } else if(atoi(right_sw) > get_swi_int(swi)){
            swi_data.relayout++;

            int fd_right_right;
            fd_right_right = open(fifo_name(cur_switch, right_switch),  O_WRONLY);
            usleep(10000);
            write(fd_right_right, message_right, strlen(message_right)+1);
            close(fd_right_right);
        } else if(atoi(right_sw) < get_swi_int(swi)){
            swi_data.relayout++;

            int fd_right_left;
            fd_right_left = open(fifo_name(cur_switch, left_switch),  O_WRONLY);
            usleep(10000);
            write(fd_right_left, message_right, strlen(message_right)+1);
            close(fd_right_left);          
        }
      }
    }

    if(fds_swi[3].revents & POLLIN){
      print_list_quit_swi(flow_table_index);
    }
  }
  free(swi_cont_fifo);
  free(cont_swi_fifo);
  return 0;
}
