#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <ctype.h>
#include <time.h>

#define NRES_TYPES  10
#define NTASKS      25

//thread
pthread_t       tid[NTASKS];
int             counter;
pthread_mutex_t lock;
//thread

int monitorTime;
int niter;
int end_state;

//counter
int resources_count;
int task_count;
//counter

char run_array[32][32];

struct resources{
  char      name[32];
  int       value;
};
struct resources resources_struct[NRES_TYPES];

struct tasks{
  char      taskName[32];
  int       busyTime;
  int       idleTime;
  char      name[32][32];
  int       value[32];
  int       taskCount;
  int       iteration;
  int       idle;
  int       run;
  double    time;
};
struct tasks tasks_struct[NTASKS];
