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

allFiles = a1mon a1jobs a1mon.c a1jobs.c Makefile README Project_Report.pdf
target=	submit
scpDir=heesoo@ug01.cs.ualberta.ca:directory
out= a2sdn
CFLAGS = -Wall -std=c99
LELIVES = -D_GNU_SOURCE

# ------------------------------------------------------------


all: a2sdn


a2sdn:
	gcc $(CFLAGS) a2sdn.c main_functions.c controller.c controller_functions.c switch.c switch_functions.c -o a2sdn $(LELIVES)

clean:
	rm $(out)
# tar:
# 	touch $(target).tar.gz
# 	mv $(target).tar.gz  x$(target).tar.gz
# 	tar -cvf $(target).tar $(allFiles)
# 	gzip $(target).tar
#
# scp:
# 	scp $(target).tar.gz $(scpDir)

# clean:
# 	rm $(out)
