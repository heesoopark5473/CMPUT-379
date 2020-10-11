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
  char    susp[2];
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

    pid_t   child1;
    pid_t   c;
    int     cstatus;
    int     exe_return;

    int     fd[2];

    pipe(fd);

    child1 = fork();

    if (child1 == 0) {
      printf("Child: PID of Child = %ld\n", (long) getpid());

      close(fd[0]);
      dup2(fd[1], STDOUT_FILENO); // I'm not checking return value for error!

      execlp(argv[0], argv[0], argv[1], argv[2] ,argv[3], argv[4], (char *) NULL);
      printf("execlp() failed.\n");
      exit(EXIT_FAILURE); // exit child process if exec fails.

    }
    pid_t child2 = fork();
    if(child2 == 0){
      // we will be reading from the pipe, no need to write:
      close(fd[1]);
      // whenever we read from stdin, actually read from the pipe.
      dup2(fd[0], STDIN_FILENO);
      // change every 's' into an 'X'
      execlp("tr", "tr","s", "X",NULL);
      // execlp() does not return (except when an error occurred)
      printf("execlp() failed.\n");
      exit(EXIT_FAILURE); // exit child process if exec fails.
    }

    	close(fd[0]);
    	close(fd[1]);
      int status;
      waitpid(child2, &status, 0);
  return 0;
}

int list_command(){
  for(int k=0; k<count; k++)
  {
      printf("%d:  (pid=      %d,  cmd=  %s,    suspended= %s\n", cmdlist[k].index, cmdlist[k].pid, cmdlist[k].prgm, cmdlist[k].susp);
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
