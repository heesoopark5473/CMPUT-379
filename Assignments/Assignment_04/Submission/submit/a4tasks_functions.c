#include "init.h"
#include "a4tasks_functions.h"

int file_exist(char* filename){
  struct stat buffer;
  return(stat(filename, &buffer) == 0);
}

void* create_task_thread(void *vargp){
  // Store the value argument passed to this thread
  int *myid = (int *)vargp;
  // Let us create a static variable to observe its changes
  static int s = 0;
  // Change static and global variables
  ++s; ++g;
  // Print the argument, static and global variables
  printf("Thread ID: %d, Static: %d, Global: %d\n", *myid, ++s, ++g);
  return NULL;
}
