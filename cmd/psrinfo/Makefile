# Write The Main Code Name Here Without Any Extensions


# Spcify Compiler
CC = gcc


# Write The Input Path for Header Files and Objects
INPUT_PATH = ./headers/


# Specify Headers : The ".h" Files for your Library 
HEADERS = $(INPUT_PATH)psrinfo_error_msg.h \
		  $(INPUT_PATH)psrinfo_functions.h \
		  $(INPUT_PATH)psrinfo_struct_init.h \
		 

# Specify Objects : The ".o" Files Generated by Compiling the ".cpp" Files
OBJECTS = $(INPUT_PATH)psrinfo_error_msg.o \
		  $(INPUT_PATH)psrinfo_functions.o \
		  $(INPUT_PATH)psrinfo_struct_init.o \
	

	
# Object File for psrinfo_functions : Containing all the essential functions
$(INPUT_PATH)psrinfo_functons.o : $(patsubst %.o, %.c, $@) $(INPUT_PATH)psrinfo_functions.h
	$(CC) -c $(patsubst %.o, %.c, $@) -o $@

# Object File for psrinfo_error_msg : containing all the error message functions in it
$(INPUT_PATH)psrinfo_error_msg.o : $(patsubst %.o, %.c, $@) $(INPUT_PATH)psrinfo_error_msg.h
	$(CC) -c $(patsubst %.o, %.c, $@) -o $@
	
# Object File for Multiple Channel Image Functions : Multiple Channel Integer Image
$(INPUT_PATH)psrinfo_struct_init.o : $(patsubst %.o, %.c, $@) $(INPUT_PATH)psrinfo_struct_init.h
	$(CC) -c $(patsubst %.o, %.c, $@) -o $@





	 
# Compilation of the Main Code
ifneq ($(MAKECMDGOALS), clean)
$(MAKECMDGOALS) : $(MAKECMDGOALS).o $(OBJECTS)
	$(CC) -o $(MAKECMDGOALS) -lm $(MAKECMDGOALS).o $(OBJECTS)  -lkstat

# Main Code Object File
$(MAKECMDGOALS).o : $(MAKECMDGOALS).c $(HEADERS) $(OPENCV_HEADERS)
	$(CC) -c $(MAKECMDGOALS).c 
endif

	


.PHONY : clean
clean :
	rm -f $(OBJECTS) *.o
