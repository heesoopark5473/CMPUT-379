# ------------------------------------------------------------
# Makefile-c: An example Makefile for CMPUT 379 (incomplete)
#
# Usage: make -f Makefile-c starter  // compile program
#        make -f Makefile-c run      // run the program
#	      	 	     	      // Note: making this target may trigger
#				      // compiling the program	 
#        make -f Makefile-c tar      // create a 'tar.gz' archive of 'allFiles'
#        make -f Makefile-c scp  // send '$(target).tar.gz' by scp to 'scpDir'
#        make -f Makefile-c clean     // remove unneeded files
# ------------------------------------------------------------

allFiles = a2sdn a2sdn.c controller.c controller.h controller_function.c controller_function.h init.h main_function.c main_function.h switch.c switch.h switch_function.c switch_function.h t1.dat t2.dat Makefile
target=	submit
scpDir=heesoo@ug01.cs.ualberta.ca:directory
out= a2sdn
CFLAGS = -Wall -std=c99
LELIVES = -D_GNU_SOURCE

# ------------------------------------------------------------


all: a2sdn


a2sdn:
	gcc $(CFLAGS) a2sdn.c main_function.c controller.c controller_function.c switch.c switch_function.c -o a2sdn $(LELIVES)

tar:
	touch $(target).tar.gz
	mv $(target).tar.gz  x$(target).tar.gz
	tar -cvf $(target).tar $(allFiles)
	gzip $(target).tar

scp:
	scp $(target).tar.gz $(scpDir)

clean:
	rm $(out)

