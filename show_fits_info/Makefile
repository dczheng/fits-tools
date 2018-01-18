CC      = gcc
CFLAGS  =  `pkg-config --cflags cfitsio`
CLIBS   =  `pkg-config --libs cfitsio` -lm
EXEC    = show_fits_info
OBJS    = show_fits_info.o
$(EXEC): $(OBJS)
	$(CC) $(OBJS) $(CLIBS) -o $(EXEC)
%.o: %.c
	$(CC)  $(CFLAGS) -c  $<
clean: 
	rm -f $(EXEC) $(OBJS)
