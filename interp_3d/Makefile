CC      = gcc
CFLAGS  =  `pkg-config --cflags cfitsio`
CLIBS   =  `pkg-config --libs cfitsio` -lm
EXEC    = interp_3d
OBJS    = interp_3d.o
$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(CLIBS) -o $(EXEC)
%.o: %.c
	$(CC)  $(CFLAGS) -c  $<
clean: 
	rm -f $(EXEC) $(OBJS)
