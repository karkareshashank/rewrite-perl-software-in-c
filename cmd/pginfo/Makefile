# Write The Main Code Name Here Without Any Extensions


# Spcify Compiler
CC = gcc


# Write The Input Path for Header Files and Objects
INPUT_PATH = ./headers/


# Specify Headers : The ".h" Files for your Library 
HEADERS = $(INPUT_PATH)pginfo_structure.h 	\
		$(INPUT_PATH)pginfo_error_msg.h 	\
		$(INPUT_PATH)pginfo_functions.h 	\
		

# Specify Objects : The ".o" Files Generated by Compiling the ".cpp" Files
OBJECTS = $(INPUT_PATH)pginfo_structure.o	\
		$(INPUT_PATH)pginfo_error_msg.o 	\
		$(INPUT_PATH)pginfo_functions.o 	\


# Object File for pginfo_structure : Initializes the structure elements
$(INPUT_PATH)pginfo_structure.o : $(patsubst %.o, %.c, $@) $(INPUT_PATH)pginfo_structure.h
	$(CC) -c $(patsubst %.o, %.c, $@) -o $@



# Object File for pginfo_error_msg : Contains all the error messages with the pginfo command
$(INPUT_PATH)pginfo_error_msg.o : $(patsubst %.o, %.c, $@) $(INPUT_PATH)pginfo_error_msg.h
	$(CC) -c $(patsubst %.o, %.c, $@) -o $@


# Object File for pginfo_functions : Contains all the functions related to pginfo
$(INPUT_PATH)pginfo_functions.o : $(patsubst %.o, %.c, $@) $(INPUT_PATH)pginfo_functions.h
	$(CC) -c $(patsubst %.o, %.c, $@) -o $@



	 
# Compilation of the Main Code
ifneq ($(MAKECMDGOALS), clean)
$(MAKECMDGOALS) : $(MAKECMDGOALS).o $(OBJECTS)
	$(CC) -o $(MAKECMDGOALS) -lm -lkstat $(MAKECMDGOALS).o $(OBJECTS)  

# Main Code Object File
$(MAKECMDGOALS).o : $(MAKECMDGOALS).c $(HEADERS)
	$(CC) -c $(MAKECMDGOALS).c 
endif

	


.PHONY : clean
clean :
	rm -f $(OBJECTS) *.o
