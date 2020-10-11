#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>
#include <pthread.h>
#include <ctype.h>

#define NRES_TYPES  10
#define NTASKS      25

int monitorTime;
int niter;
int g;

int resource_count;

struct resources{
  char      name[32];
  int       value;
};
struct resources resources_struct[NRES_TYPES];

struct tasks{
  char      taskName[32];
  int       busyTime;
  int       idleTime;
  char      name_value[32][32];
};
struct tasks tasks_struct[NTASKS];
