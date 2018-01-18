CC      = gcc
CFLAGS  =  `pkg-config --cflags cfitsio gsl`
CLIBS   =  `pkg-config --libs cfitsio gsl` -lm
EXEC    = interp_3d
OBJS    = interp_3d.o
$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(CLIBS) -o $(EXEC)
	rm -f $(OBJS)
%.o: %.c
	$(CC)  $(CFLAGS) -c  $<
clean: 
	rm -f $(EXEC) $(OBJS)
