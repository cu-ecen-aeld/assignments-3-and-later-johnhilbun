# Initial work based on https://www.gnu.org/software/make/manual/html_node/Simple-Makefile.html

all: aesdsocket
default: aesdsocket

# whether to cross compile or not is based on command line argument
ifeq ($(CROSS_COMPILE),aarch64-none-linux-gnu-)
CC = $(CROSS_COMPILE)gcc
else
CC = gcc
endif

aesdsocket: aesdsocket.o

aesdsocket.o: aesdsocket.c
	$(CC) -c aesdsocket.c

# -f : won't give error if no file exists
clean: 
	rm -f aesdsocket *.o

