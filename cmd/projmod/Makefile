# Write The Main Code Name Here Without Any Extensions


# Spcify Compiler
CC = gcc


# Write The Input Path for Header Files and Objects
INPUT_PATH = ./headers/


# Specify Headers : The ".h" Files for your Library 
HEADERS = $(INPUT_PATH)project_file_check.h \
		$(INPUT_PATH)projmod_structure.h 	\
		$(INPUT_PATH)projmod_functions.h	\
		$(INPUT_PATH)projmod_error_msg.h	 \

# Specify Objects : The ".o" Files Generated by Compiling the ".cpp" Files
OBJECTS = $(INPUT_PATH)project_file_check.o \
		$(INPUT_PATH)projmod_structure.o	\
		$(INPUT_PATH)projmod_error_msg.o \
		$(INPUT_PATH)projmod_functions.o	\
	

	
# Object File for project_file_check : Checks the project file for errors
$(INPUT_PATH)project_file_check.o : $(patsubst %.o, %.c, $@) $(INPUT_PATH)project_file_check.h
	$(CC) -c $(patsubst %.o, %.c, $@) -o $@


# Object File for project_structure : Initializes the structure elements
$(INPUT_PATH)projmod_structure.o : $(patsubst %.o, %.c, $@) $(INPUT_PATH)projmod_structure.h
	$(CC) -c $(patsubst %.o, %.c, $@) -o $@


# Object File for project_error_msg : Contains all the error messages to be printed
$(INPUT_PATH)projmod_error_msg.o : $(patsubst %.o, %.c, $@) $(INPUT_PATH)projmod_error_msg.h
	$(CC) -c $(patsubst %.o, %.c, $@) -o $@

# Object File for project_functions : Contain all the functions needed to perform projmod
$(INPUT_PATH)projmod_functions.o : $(patsubst %.o, %.c, $@) $(INPUT_PATH)projmod_functions.h
	$(CC) -c $(patsubst %.o, %.c, $@) -o $@


	 
# Compilation of the Main Code
ifneq ($(MAKECMDGOALS), clean)
$(MAKECMDGOALS) : $(MAKECMDGOALS).o $(OBJECTS)
	$(CC) -o $(MAKECMDGOALS) -lm -lproject $(MAKECMDGOALS).o $(OBJECTS)  

# Main Code Object File
$(MAKECMDGOALS).o : $(MAKECMDGOALS).c $(HEADERS) 
	$(CC) -c $(MAKECMDGOALS).c 
endif

	


.PHONY : clean
clean :
	rm -f $(OBJECTS) *.o
