#include "init.h"

char *trimwhitespace(char* str);
void store_resources(char* message, int index);

int a4_resources(char* line){
    char resources[32];
    char resources_array[32][32];
    strcpy(resources, line);

    char* token = strtok(resources, " ");
    resource_count = 0;

    while(token != NULL){
      if(strncmp(token, "resources", strlen("resources")) != 0){
        token = trimwhitespace(token);
        strcpy(resources_array[resource_count], token);
        resource_count++;
      }
      token = strtok(NULL, " ");
    }

    for(int k=0; k<resource_count; k++){
      store_resources(resources_array[k], k);
    }
    return 0;
}


void store_resources(char* message, int index){
  char* name  = strtok(message, ":");
  char* value = strtok(NULL, ":");

  strcpy(resources_struct[index].name, name);
  resources_struct[index].value = atoi(value);
}

char *trimwhitespace(char *str)
{
  char *end;
  // Trim leading space
  while(isspace((unsigned char)*str)) str++;

  if(*str == 0)  // All spaces?
    return str;
  // Trim trailing space
  end = str + strlen(str) - 1;
  while(end > str && isspace((unsigned char)*end)) end--;

  // Write new null terminator character
  end[1] = '\0';

  return str;
}
