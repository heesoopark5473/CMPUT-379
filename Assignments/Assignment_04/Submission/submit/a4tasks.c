#include "init.h"
#include "a4tasks_functions.h"
#include "a4_resources.h"
#include "a4_tasks.h"

int main(int argc, char* argv[]){
  if(argc != 4){
    printf("Invalid Number of Inputs\n");
    return 0;
  }

  FILE* file;
  char  file_array[1024][1024];
  char  line_read[1024];
  int   line_count = 0;

  if(file_exist(argv[1])){file = fopen(argv[1], "r");}
  else{printf("\n\tinputFile Does Not Exist\n\n"); return 0;}

  while(fgets(line_read, sizeof(line_read), file) != NULL) {
      if(strncmp(line_read, "\n", 1) != 0 && strncmp(line_read, "#", 1) != 0){strcpy(file_array[line_count], line_read); line_count++;}
  }

  for(int j=0; j<line_count+1; j++){
    if(strncmp(file_array[j], "resources", strlen("resources")) == 0){
      a4_resources(file_array[j]);
    }else if(strncmp(file_array[j], "task", strlen("task")) == 0){
      a4_tasks(file_array[j], j);
    }
  }
  return 0;
}
