##Adapted from http://www.cs.colby.edu/maxwell/courses/tutorials/maketutor/
CC		=	gcc
CFLAGS	=	-Wall
OBJ 	= 	server.o respond.o
EXE 	= 	server


##Create executable linked file from object files.
$(EXE): $(OBJ)
	$(CC) $(CFLAGS) -o $(EXE) $(OBJ)

respond.o: respond.h

##Delete object files
clean:
	/bin/rm $(OBJ)

##Performs clean (i.e. delete object files) and deletes executable
clobber: clean
	/bin/rm $(EXE)
