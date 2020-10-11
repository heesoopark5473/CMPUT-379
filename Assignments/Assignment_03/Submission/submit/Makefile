allFiles 	= a3sdn
target 		= submit
scpDir 		= heesoo@ug01.cs.ualberta.ca:directory
out 		= a3sdn myfifo*
CFLAGS		= -Wall -std=c99
LELIVES 	= -D_GNU_SOURCE

all: a3sdn

a3sdn:
	gcc $(CFLAGS) -pthread a3sdn.c a3sdn_functions.c controller.c controller_functions.c switch.c switch_functions.c -o a3sdn $(LELIVES)

clean:
	rm $(out)
