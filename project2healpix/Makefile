CC      = gcc
CFLAGS  =  `pkg-config --cflags cfitsio`
CLIBS   =  `pkg-config --libs cfitsio` -lm -lchealpix
EXEC    = project2healpix
OBJS    = project2healpix.o
$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(CLIBS) -o $(EXEC)
%.o: %.c
	$(CC)  $(CFLAGS) -c  $<
clean: 
	rm -f $(EXEC) $(OBJS)
