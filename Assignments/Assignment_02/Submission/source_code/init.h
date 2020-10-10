#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <poll.h>
#include <unistd.h>
#include <fcntl.h>

#define MAX_NSW 7
#define MAXIP 1000
#define TIMEOUT 100
#define SRCIP_LO 0
#define SRCIP_HI 1000
#define UNUSED(x) (void)(x)
