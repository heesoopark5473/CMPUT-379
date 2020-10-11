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
#include <fcntl.h>
#include <signal.h>
#define STD_OUT 1

int     setrlimit_func();
int     parse_command(char* input);
int     run_command(char* prgm, char* p1, char* p2, char* p3, char* p4);
int     list_command();
int     suspend_command(char* jobNo);
int     resume_command(char* jobNo);
int     terminate_command(char* jobNo);
int     exit_command();
int     quit_command();
int     iadd(int count);
int     isub(int count);
char*   get_command();
char*   trim(char *str);

struct command_struct {
  int     index;
  int     pid;
  int     ppid;
  char    prgm[20];
};
struct command_struct cmdlist[100];

int setrlimit_func() {
  struct    rlimit r_limit;
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
  printf("a1jobs[%d] : ", getpid());
  fgets (user_input, 100, stdin);
  return user_input;
}

int parse_command(char* input) {
  char*     parameter = strtok(input, " ");
  char*     array[6];
  int       i;
  int       cmd_return;
  int       ret_list, ret_run, ret_suspend, ret_resume, ret_terminate, ret_exit, ret_quit;
  char*     command_trim;
  char*     p1_trim;
  char*     p2_trim;
  char*     p3_trim;
  char*     p4_trim;
  char*     p5_trim;

  for(i=0; i<6; i++) {
    array[i] = parameter;
    parameter = strtok (NULL, " ");
  }

  command_trim = trim(array[0]);
  p1_trim = trim(array[1]);
  p2_trim = trim(array[2]);
  p3_trim = trim(array[3]);
  p4_trim = trim(array[4]);
  p5_trim = trim(array[5]);

  ret_list = strncmp(command_trim, "list", strlen(command_trim));
  ret_run = strncmp(command_trim, "run", strlen(command_trim));
  ret_suspend = strncmp(command_trim, "suspend", strlen(command_trim));
  ret_resume = strncmp(command_trim, "resume", strlen(command_trim));
  ret_terminate = strncmp(command_trim, "terminate", strlen(command_trim));
  ret_exit = strncmp(command_trim, "exit", strlen(command_trim));
  ret_quit = strncmp(command_trim, "quit", strlen(command_trim));

  if(ret_list == 0){
    list_command();
    cmd_return = 0;
  }else if(ret_run == 0){
    run_command(p1_trim, p2_trim, p3_trim, p4_trim, p5_trim);
    cmd_return = 1;
  }else if(ret_suspend == 0){
    suspend_command(p1_trim);
    cmd_return = 2;
  }else if(ret_resume == 0){
    resume_command(p1_trim);
    cmd_return = 3;
  }else if(ret_terminate == 0){
    terminate_command(p1_trim);
    cmd_return = 4;
  }else if(ret_exit == 0){
    exit_command();
    cmd_return = 5;
  }else if(ret_quit == 0){
    cmd_return = 6;
  }else{
    fprintf(stdout, "\n\t[*] Invalid Command Entered [*]\t\n");
    cmd_return = 7;
  }
  return cmd_return;
}

int count;

int run_command(char* prgm, char* p1, char* p2, char* p3, char* p4) {
    char* argv[5];
    argv[0] = prgm;
    argv[1] = p1;
    argv[2] = p2;
    argv[3] = p3;
    argv[4] = p4;

    pid_t   child;
    pid_t   c;
    int     cstatus;
    int     exe_return;

    int     mypipe[2];

    pipe(mypipe);

    child = fork();

    if (child == 0) {
      close(mypipe[0]);
      dup2(mypipe[1], STD_OUT);

      exe_return = execlp(argv[0], argv[0], argv[1], argv[2] ,argv[3], argv[4], (char *) NULL);
      close(mypipe[1]);


      exit(1);

    } else { /* Parent process. */
      char buf[1024];
      int bytes_read;
      close(mypipe[1]);
      while ((bytes_read = read(mypipe[0], buf, 1024)) > 0) {
        write(STD_OUT, buf, bytes_read); //write from buf to STD_OUT
        printf("%s\n", buf);
    }
      wait(NULL);
      close(mypipe[0]);

  }
  return 0;
}

int list_command(){
  for(int k=0; k<count; k++)
  {
      printf("%d:  (pid=      %d,  cmd=  %s\n", cmdlist[k].index, cmdlist[k].pid, cmdlist[k].prgm);
  }
  return 0;
}

int suspend_command(char * jobNo){
  int jobNo_int = atoi(jobNo);
  for(int k=0; k<count; k++)
  {
      if(cmdlist[k].index == jobNo_int){
        kill(cmdlist[k].pid, SIGSTOP);
        //strcpy(cmdlist[k].status, "Suspended");
        printf("\nPARENT: sending SIGSTOP to suspend the process : %d\n\n", cmdlist[k].index);
        printf("\nPARENT: sending SIGSTOP to suspend the process : %d\n\n", cmdlist[k].pid);
        //printf("[%d] pid : %ld, ppid : %d, prgm : %s\n", cmdlist[k].index, cmdlist[k].pid, cmdlist[k].ppid, cmdlist[k].prgm);
      }
  }
  return 0;
}

int resume_command(char* jobNo){
  int jobNo_int = atoi(jobNo);
  for(int k=0; k<count; k++)
  {
      if(cmdlist[k].index == jobNo_int){
        kill(cmdlist[k].pid, SIGCONT);
        //strcpy(cmdlist[k].status, "Running");
        printf("\nPARENT: sending SIGCONT to suspend the process : %d\n\n", cmdlist[k].index);
        printf("\nPARENT: sending SIGCONT to suspend the process : %d\n\n", cmdlist[k].pid);
        //printf("[%d] pid : %ld, ppid : %d, prgm : %s\n", cmdlist[k].index, cmdlist[k].pid, cmdlist[k].ppid, cmdlist[k].prgm);
      }
  }
  return 0;
}

int terminate_command(char* jobNo){
  int jobNo_int = atoi(jobNo);
  printf("\nCOUNT: %dn", count);
  for(int k=0; k<count; k++)
  {

      if(cmdlist[k].index == jobNo_int){
        printf("\nPARENT: sending SIGKILL to suspend the process : %d\n\n", cmdlist[k].index);
        printf("\nPARENT: sending SIGKILL to suspend the process : %d\n\n", cmdlist[k].pid);
        kill(cmdlist[k].pid, SIGKILL);
        //strcpy(cmdlist[k].status, "Terminated");
        for (int t=k; t<count+1; t++){
          cmdlist[t].index = cmdlist[t].index;
          cmdlist[t].pid = cmdlist[t+1].pid;
          cmdlist[t].ppid = cmdlist[t+1].ppid;
          //strcpy(cmdlist[count].status, "Running");
          strcpy(cmdlist[t].prgm, cmdlist[t+1].prgm);
          k = count;
        }
        //printf("[%d] pid : %ld, ppid : %d, prgm : %s\n", cmdlist[k].index, cmdlist[k].pid, cmdlist[k].ppid, cmdlist[k].prgm);
      }
  }
  count = isub(count);

  return 0;
}

int exit_command(){
  int status;
  for(int k=0; k<count; k++){
    kill(cmdlist[k].pid, SIGKILL);
    waitpid(cmdlist[k].pid, &status, 0);
    printf("\t\tjob %d terminated\n", cmdlist[k].pid);
    //printf("Clean Process : %d\n", cmdlist[k].pid);
    //write(1,"cleanup!\n",9);
  }
  return 0;
}

int quit_command(){
  return 0;
}

int iadd(i)
{
  i +=1;
  return(i);
}
int isub(i)
{
  i -=1;
  return(i);
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

int main() {
  setrlimit_func();
  char* input;
  while(1) {
    input = get_command();
    int func_option;
    func_option = parse_command(input);
    if (func_option == 5){
      break;
    }else if(func_option == 6){
      break;
    }else{
      continue;
    }
  }
  return 0;
}
