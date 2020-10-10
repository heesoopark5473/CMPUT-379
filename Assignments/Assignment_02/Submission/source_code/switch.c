#include "init.h"
#include "switch_functions.h"
#include "switch_structs.h"
#include "controller_functions.h"

int switch_sdn(char* swi, char* trafficFile, char* swj, char* swk, char* IP_range){
  FILE* file;
  char  file_array[124][124];
  int i = 0;
  int iter_count=0;
  flow_table_index=0;


  if(file_exist(trafficFile)){file=fopen(trafficFile,"r");}
  else{printf("trafficFile Does Not Exist\n"); return 0;}

  while(fgets(file_array[i], sizeof(file_array[i]), file)){ i++; }

  char* swi_cont_fifo = fifo_name(get_swi_int(swi), 0);
  char* cont_swi_fifo = fifo_name(0, get_swi_int(swi));

  printf("\nSwitch Reads From   : %s\n", cont_swi_fifo);
  printf("Switch Writes To    : %s\n\n", swi_cont_fifo);

  update_flowtable(flow_table_index, 0, MAXIP, IP_range, "Forward", 3, 4, 0);

  int fd_write;

  while(1){
    char* get_open;
    printf("%d\n", iter_count);
    get_open = get_open_pkt(swi, swj, swk, IP_range);

    if(iter_count == 0){
      if(file_exist(cont_swi_fifo) && file_exist(swi_cont_fifo)){
        printf("OPEN SENT\n");
        fd_write =  open(swi_cont_fifo, O_WRONLY);
        write(fd_write, get_open, strlen(get_open)+1);
        close(fd_write);
        free(get_open);
      }else{ printf("\nPORT 0 NOT CONNECTED -- FIFO HAVE NOT BEEN CREATED\n"); }
    }
    iter_count++;
  }
  return 0;
}
