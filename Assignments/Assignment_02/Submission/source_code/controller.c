#include "init.h"
#include "controller_functions.h"

int controller(int nSwitch){
  char  fifo_array[nSwitch][2][32];
  char* swi_cont_fifo;
  char* cont_swi_fifo;

  remove_fifo();

  create_switch_fifo(nSwitch);

  for(int i=0; i<nSwitch; i++){
    swi_cont_fifo = fifo_name(i+1, 0);
    cont_swi_fifo = fifo_name(0, i+1);
    mkfifo(swi_cont_fifo, 0666);
    mkfifo(cont_swi_fifo, 0666);
    strcpy(fifo_array[i][0], swi_cont_fifo);
    strcpy(fifo_array[i][1], cont_swi_fifo);
    free(swi_cont_fifo);
    free(cont_swi_fifo);
  }

  while(1){
    for(int k=0; k<nSwitch; k++){
      struct pollfd fds_cont[2];

      int ret_cont;
      int fd_read;
      int fd_write;
      char message[32];
      char message_copy[32];

      fd_read = open(fifo_array[k][0], O_RDONLY | O_NONBLOCK);

      //read
      fds_cont[0].fd        = fd_read;
      fds_cont[0].events    = POLLIN;
      //write
      fds_cont[1].fd        = fd_write;
      fds_cont[1].events    = POLLIN;

      ret_cont = poll(fds_cont, 2, TIMEOUT);

      if(ret_cont == -1){ perror("poll"); }
      if(!ret_cont){printf("%s\n", fifo_array[k][0]);}
      if(fds_cont[0].revents & POLLIN){
        read(fd_read, message, sizeof(char)*32);
      }
    }
  }

  return 0;
}
