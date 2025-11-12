# based on https://www.gnu.org/software/make/manual/html_node/Simple-Makefile.html

writer : writer.o

writer.o : writer.c
	gcc -c writer.c

# -f : won't give error if no file exists
clean :
	rm -f writer *.o

