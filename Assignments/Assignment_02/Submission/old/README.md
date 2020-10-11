
    CMPUT 379 OPERATING SYSTEM CONCEPTS
    Assignment #2
    
    Due: Thursday, November 1, 2018, 09:00 PM
    Modeling a Software Defined Netwrok using FIFOs
    Name:   Hee Soo Park
    CCID:   1389563


README

(1) Please move directory to a directory of my submition

(2) Please unzip and go to the unziped folder directory

(3) in terminal, "make clean" to remove all uncessary files

(2) in terminal, "make" to compile "a2sdn" with correct CFLAGS

(3) in terminal, run "./a2sdn cont nSwitchs"

(4) in terminal, run "./a2sdn swi trafficFile swj swk IP_range"
		 (please run switches in increasing order, e.g., sw1 -> sw2 -> sw3 ...)
		 (please run with correct swj swk switches)

(5) Please watch out for invalid switch input
	(e.g., if sw1 -> swj should be "null", if sw == nSwitch -> swk should be null)
	(e.g., if nSwitch == 3, swj (1) <- swi (2) -> swk(3))
	(e.g., if switch 1 have been executed, then when executing switch 2, swj should be sw1)

(4) once a2sdn have sucessfully executed, please run desired processes

(5)	a2sdn terminal commands
 	List: "list"  -> list the status of controller/switch	
	Exit: "exit"  -> list the status of controller/switch and exit

(6) to execute a2sdn, after compiling with makefile, check if executable "a2sdn" exists


Files:

a2sdn.c      : Main function of the program, error checking and executes controller or switch depending of user commnad

main_function.c : all the functions required to run a2sdn.c

controller.c : controller function

switch.c     : switch function

switch_function.c : all the functions required to run switch.c

controller_function.c : all the functions required to run controller.c

controller.h : headerfile for controller.c (declaration of all the functions in controller.c)

switch.h     : headerfile for switch.c (declaration of all the functions in switch.c)

switch_function.h : headerfile for switch_function.c (declaration of all the functions in switch_function.c)

init.h	: declaration of headerfiles and declaration of structs and global variable used throughout the program.


Thank you,
