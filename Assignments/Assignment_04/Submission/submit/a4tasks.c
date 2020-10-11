#include "init.h"
#include "a4tasks_functions.h"

int main(int argc, char* argv[]){
  if(argc != 4){printf("\n\tInvalid Number of Arguments\n"); return 0;}

  FILE*   file;
  char    file_array[1024][1024];
  char    line_read[1024];
  int     line_count = 0;

  end_state = 0;

  if(file_exists(argv[1])){file = fopen(argv[1], "r");}
  else{printf("\n\tInputFile Does Not Exists\n\n"); return 0;}

  while(fgets(line_read, sizeof(line_read), file) != NULL) {
      if(strncmp(line_read, "\n", 1) != 0 && strncmp(line_read, "#", 1) != 0){strcpy(file_array[line_count], line_read); line_count++;}
  }

  for(int i=0; i<line_count+1; i++){
    if(strncmp(file_array[i], "resources", strlen("resources")) == 0){store_resources(file_array[i]);}
  }

  monitorTime = atoi(argv[2]);
  niter       = atoi(argv[3]);

  task_count = 0;
  for(int j=0; j<line_count+1; j++){
    if(strncmp(file_array[j], "task", strlen("task")) == 0){
      store_tasks(file_array[j], task_count);
      task_count++;
    }
  }

  if(pthread_mutex_init(&lock, NULL) != 0){printf("\nmutex init failed\n");return 1;}

  int thread_count = 0;
  int err;

  pthread_t m_tid;
  int *ptr  = malloc(sizeof(*ptr));
  *ptr      = atoi(argv[2]);
  pthread_create(&m_tid, NULL, monitor_thread, ptr);

  while(thread_count < task_count){
    int *arg  = malloc(sizeof(*arg));
    *arg      = thread_count;
    err       = pthread_create(&(tid[thread_count]), NULL, &task_thread, arg);
    if(err != 0){printf("\ncan't create thread : [%s]", strerror(err));}
    thread_count++;
  }

  for(int h=0; h<task_count; h++){pthread_join(tid[h], NULL);}

  pthread_mutex_destroy(&lock);

  return 0;
}
