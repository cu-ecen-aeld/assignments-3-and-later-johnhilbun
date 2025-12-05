# Initial work based on https://www.gnu.org/software/make/manual/html_node/Simple-Makefile.html
all: writer

# whether to cross compile or not is based on command line argument
ifeq ($(CROSS_COMPILE),aarch64-none-linux-gnu-)
CC = $(CROSS_COMPILE)gcc
else
CC = gcc
endif

writer: writer.o

writer.o: writer.c
	$(CC) -c writer.c

# -f : won't give error if no file exists
clean: 
	rm -f writer *.o

