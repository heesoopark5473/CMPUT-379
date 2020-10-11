allFiles = a4tasks a4tasks.c a4tasks_functions.c a4tasks_functions.h init.h Makefile README ProjectReport.pdf
target=	submit
scpDir=heesoo@ug01.cs.ualberta.ca:directory
out= a4tasks
CFLAGS = -Wall -std=c99 -lpthread
LELIVES = -D_GNU_SOURCE

# ------------------------------------------------------------


all: a4tasks

a4tasks:
	gcc $(CFLAGS) a4tasks.c a4tasks_functions.c -o a4tasks $(LELIVES)

tar:
	touch $(target).tar.gz
	mv $(target).tar.gz  x$(target).tar.gz
	tar -cvf $(target).tar $(allFiles)
	gzip $(target).tar

scp:
	scp $(target).tar.gz $(scpDir)

clean:
	rm $(out)
