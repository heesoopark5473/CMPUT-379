#include <stdio.h>
#include <stdlib.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/times.h>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <fcntl.h> // library for fcntl function
#include <signal.h>

int setrlimit_function();
char* get_command();
char* trim(char *str);

int setrlimit_function(){
  struct rlimit r_limit;

  getrlimit(RLIMIT_CPU, &r_limit);
  //printf("\n Default value is : %lld\n", (long long int)r_limit.rlim_cur);

  r_limit.rlim_cur = 600;

  setrlimit(RLIMIT_CPU, &r_limit);
  getrlimit(RLIMIT_CPU, &r_limit);
  //printf("\n Dafault value now is : %lld\n", (long long int)r_limit.rlim_cur);
  return 0;
}

char* get_command() {
  static char     user_input[100];
  pid_t           parent_pid;
  usleep(200000);
  parent_pid = getpid();
  printf("a1mon ");
  fgets (user_input, 100, stdin);
  return user_input;
}

int parse_command(char* input) {
  char*     parameter = strtok(input, " ");
  char*     array[2];
  char*     p1_trim;
  char*     p2_trim;
  int       i;

  for(i=0; i<2; i++){
    array[i] = parameter;
    parameter = strtok (NULL, " ");
  }

  p1_trim = trim(array[0]);
  p2_trim = trim(array[1]);

  printf("\n%s %s\n", p1_trim, p2_trim);
  return 0;
}

//Referenced: https://stackoverflow.com/questions/122616/how-do-i-trim-leading-trailing-whitespace-in-a-standard-way?rq=1
char *trim(char *str)
{
    size_t len = 0;
    char *frontp = str;
    char *endp = NULL;
    if( str == NULL ) { return NULL; }
    if( str[0] == '\0' ) { return str; }
    len = strlen(str);
    endp = str + len;
    while( isspace((unsigned char) *frontp) ) { ++frontp; }
    if( endp != frontp )
    {
        while( isspace((unsigned char) *(--endp)) && endp != frontp ) {}
    }
    if( str + len - 1 != endp )
            *(endp + 1) = '\0';
    else if( frontp != str &&  endp == frontp )
            *str = '\0';
    endp = str;
    if( frontp != str )
    {
            while( *frontp ) { *endp++ = *frontp++; }
            *endp = '\0';
    }
    return str;
}


int main(){

  setrlimit_function();
  char* input;
  FILE *open_file;
  while(1){
    //input = get_command();
    //parse_command(input);
    open_file = popen("ps -T -u $USER -o user,pid,ppid,state,start,cmd --sort start", "r");
    //printf("\n%s\n", open_file);
    //pclose(open_file);
  }
  //printf("NOT DONE YET\n");
  return 0;
}
