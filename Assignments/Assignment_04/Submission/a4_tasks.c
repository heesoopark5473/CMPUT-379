#include "init.h"

void* task_busyTime(void *ptr);

int a4_tasks(char* line, int index){
  char tasks[32];
  char tasks_array[32][32];
  strcpy(tasks, line);

  char* token = strtok(tasks, " ");
  int count = 0;

  while(token != NULL){
      if(strncmp(token, "task", strlen("task")) != 0){
        strcpy(tasks_array[count], token);
        count++;
      }
      token = strtok(NULL, " ");
  }

  int name_count = 0;

  for(int k=0; k<count; k++){
    if(k == 0){
      strcpy(tasks_struct[index].taskName, tasks_array[k]);
    }else if(k==1){
      tasks_struct[index].busyTime = atoi(tasks_array[k]);
    }else if(k==2){
      tasks_struct[index].idleTime = atoi(tasks_array[k]);
    }else{
      strcpy(tasks_struct[index].name_value[k], tasks_array[k]);
      name_count++;
    }
  }

  char name_value[32];
  int  flag_count = 0;
  char* value;
  char* name;
  char temp_array[32][32][32];

  for(int e=0; e<resource_count; e++){
    for(int t=3; t<name_count+3; t++){
      strcpy(name_value, tasks_struct[index].name_value[t]);
      name  = strtok(name_value, ":");
      value = strtok(NULL, ":");
      if(strncmp(resources_struct[e].name, name, strlen(name)) == 0 && resources_struct[e].value >= atoi(value)){
        strcpy(temp_array[flag_count][0], name);
        strcpy(temp_array[flag_count][1], value);
        flag_count++;
      }
    }
  }

  if(flag_count == name_count){
    for(int b=0; b<flag_count; b++){
      for(int v=0; v<resource_count; v++){
        if(strncmp(resources_struct[v].name, temp_array[b][0], strlen(temp_array[b][0])) == 0){
          resources_struct[v].value = resources_struct[v].value - atoi(temp_array[b][1]);
        }
      }
    }

    char str[32];
    sprintf(str, "%d", tasks_struct[index].busyTime);
    pthread_t thread_busyTime;
    pthread_create(&thread_busyTime, NULL, task_busyTime, (void*) str);

  }

  return 0;
}

void* task_busyTime(void *ptr){
  char* temp_char;
  temp_char = (char *)ptr;
  int busyTime = atoi(temp_char)*1000;
  printf("\n\t** Entering a delay period of %d mesc\n\n", busyTime/1000);
  usleep(busyTime);
  printf("\n\t** Delay period ended\n\n");
  return NULL;
}
