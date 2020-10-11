#include "init.h"

int file_exists(char* filename){
  struct stat buffer;
  return(stat(filename, &buffer) == 0);
}

char *trimwhitespace(char *str){
  char *end;
  while(isspace((unsigned char)*str)) str++;
  if(*str == 0)
    return str;
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;
  end[1] = '\0';
  return str;
}

void store_resources(char* line){
  char resources[32];
  char resources_array[32][32];
  strcpy(resources, line);
  char* token = strtok(resources, " ");
  resources_count = 0;
  while(token != NULL){
    token = trimwhitespace(token);
    if(strncmp(token , "resources", strlen("resources")) != 0){
      strcpy(resources_array[resources_count], token);
      resources_count++;
    }
    token = strtok(NULL, " ");
  }
  for(int i=0; i<resources_count; i++){
    char* name  = strtok(resources_array[i], ":");
    char* value = strtok(NULL, ":");
    strcpy(resources_struct[i].name, name);
    resources_struct[i].value = atoi(value);
  }
}

void store_tasks(char* line, int index){
  char tasks[32];
  char tasks_array[32][32];
  strcpy(tasks,line);
  char* token = strtok(tasks, " ");
  int task_item_count = 0;
  while(token != NULL){
    token = trimwhitespace(token);
    if(strncmp(token, "task", strlen("task")) != 0){
      strcpy(tasks_array[task_item_count], token);
      task_item_count++;
    }
    token = strtok(NULL, " ");
  }
  int task_resource_count = 0;
  for(int i=0; i<task_item_count; i++){
    if(i==0){strcpy(tasks_struct[index].taskName, tasks_array[i]);}
    else if(i==1){tasks_struct[index].busyTime = atoi(tasks_array[i]);}
    else if(i==2){tasks_struct[index].idleTime = atoi(tasks_array[i]);}
    else{
      char* name  = strtok(tasks_array[i], ":");
      char* value = strtok(NULL, ":");
      strcpy(tasks_struct[index].name[task_resource_count], name);
      tasks_struct[index].value[task_resource_count] = atoi(value);
      task_resource_count++;
    }
  }
  tasks_struct[index].taskCount = task_resource_count;
}

void* monitor_thread(void* arg){
  int m_time = *((int *) arg);
  free(arg);

  char idle_array[1024][32];
  char run_array[1024][32];
  char wait_array[1024][32];

  while(1){
    if(end_state == 1){
      printf("\n\nOutput of the termination phase\n");
      printf("================================\n");
      printf("System Resources:\n");
      for(int z=0; z<resources_count; z++){
        printf("\t%s: (maxAvail=\t%d, held=\t 0)\n", resources_struct[z].name, resources_struct[z].value);
      }
      printf("\nSystem Tasks:\n");
      for(int c=0; c<task_count; c++){
        printf("[%d] %s (IDLE, runTime= %d mesc, idleTime= %d mesc): \n", c, tasks_struct[c].taskName, tasks_struct[c].busyTime, tasks_struct[c].idleTime);
        int *myid = (int *)&tid[c];
        printf("\t(tid= %d)\n", *myid);
        for(int m=0; m<tasks_struct[c].taskCount; m++){
          printf("\t%s: (needed=\t%d, held=\t0)\n", tasks_struct[c].name[m], tasks_struct[c].value[m]);
        }
        printf("\tRUN: %d times, WAIT: 0 mesc\n\n", tasks_struct[c].iteration);
      }
      exit(1);
    }

    int idle_count = 0;
    int run_count = 0;
    int wait_count = 0;

    usleep(m_time*1000);
    printf("\nMonitor: ");
    for(int x=0; x<task_count; x++){
      if(tasks_struct[x].idle == 1){
        strcpy(idle_array[idle_count], tasks_struct[x].taskName);
        idle_count++;
      }else if(tasks_struct[x].run ==1){
        strcpy(run_array[run_count], tasks_struct[x].taskName);
        run_count++;
      }else{
        strcpy(wait_array[wait_count], tasks_struct[x].taskName);
        wait_count++;
      }
    }

    printf("[WAIT] ");
    for(int n=0; n<wait_count; n++){
      printf("%s ", wait_array[n]);
    }
    printf("\n\t [IDLE] ");
    for(int g=0; g<idle_count; g++){
      printf("%s ", idle_array[g]);
    }
    printf("\n\t [RUN]  ");
    for(int h=0; h<run_count; h++){
      printf("%s ", run_array[h]);
    }
    printf("\n");
  }
  return NULL;
}

void* task_thread(void* arg){
  int index = *((int *) arg);
  free(arg);

  while(1){

    clock_t begin = clock();

    if(tasks_struct[index].idle == 1){
      usleep(tasks_struct[index].idleTime*1000);
      int *myid = (int *)&tid[index];
      clock_t end = clock();
      double time_spent = (double)(end-begin) / CLOCKS_PER_SEC;
      tasks_struct[index].time = tasks_struct[index].time + time_spent;
      printf("task: %s (tid= %d, iter= %d, time =  %f mesc)\n", tasks_struct[index].taskName, *myid, tasks_struct[index].iteration, tasks_struct[index].time*1000000);
      tasks_struct[index].idle = 0;
    }else{
      pthread_mutex_lock(&lock);

      int task_count = 0;

      for(int i=0; i<resources_count; i++){
        for(int j=0; j<tasks_struct[index].taskCount; j++){
          if(strncmp(resources_struct[i].name, tasks_struct[index].name[j], strlen(tasks_struct[index].name[j])) == 0){
            if(resources_struct[i].value >= tasks_struct[index].value[j]){
              task_count++;
            }
          }
        }
      }

      if(task_count == tasks_struct[index].taskCount){
        for(int x=0; x<resources_count; x++){
          for(int y=0; y<task_count; y++){
            if(strncmp(resources_struct[x].name, tasks_struct[index].name[y], strlen(tasks_struct[index].name[y])) == 0){
              resources_struct[x].value = resources_struct[x].value - tasks_struct[index].value[y];
              strcpy(run_array[y], tasks_struct[index].taskName);
            }
          }
        }

        tasks_struct[index].run = 1;
        usleep(tasks_struct[index].busyTime*1000);

        for(int a=0; a<resources_count; a++){
          for(int b=0; b<task_count; b++){
            if(strncmp(resources_struct[a].name, tasks_struct[index].name[b], strlen(tasks_struct[index].name[b])) == 0){
              resources_struct[a].value = resources_struct[a].value + tasks_struct[index].value[b];
            }
          }
        }
        
        tasks_struct[index].run = 0;
        tasks_struct[index].idle = 1;
        tasks_struct[index].iteration++;

        if(tasks_struct[index].iteration == niter){
          end_state = 1;
          return NULL;
        }
        pthread_mutex_unlock(&lock);
      }
    }
  }
  return NULL;
}
