allFiles 	= a3sdn a3sdn.c a3sdn_functions.c a3sdn_functions.h controller.c controller.h controller_functions.c controller_functions.h switch.c switch.h switch_functions.c switch_functions.h init.h Makefile README ProjectReport.pdf
target 		= submit
scpDir 		= heesoo@ug01.cs.ualberta.ca:directory
out 		= a3sdn myfifo*
CFLAGS		= -Wall -std=c99
LELIVES 	= -D_GNU_SOURCE

all: a3sdn

a3sdn:
	gcc $(CFLAGS) -pthread a3sdn.c a3sdn_functions.c controller.c controller_functions.c switch.c switch_functions.c -o a3sdn $(LELIVES)

tar:
	touch $(target).tar.gz
	mv $(target).tar.gz  x$(target).tar.gz
	tar -cvf $(target).tar $(allFiles)
	gzip $(target).tar

scp:
	scp $(target).tar.gz $(scpDir)	

clean:
	rm $(out)
