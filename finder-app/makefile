# based on https://www.gnu.org/software/make/manual/html_node/Simple-Makefile.html

# whether to cross compile or not is based on command line argument
cross-target = aarch64-none-linux-gnu-
ifeq ($(CROSS_COMPILE),cross-target)
	@echo "will cross compile"
	CC := $(cross-target)gcc
else
	# use GCC
	#@echo "native compilation"
	CC := gcc
endif

all:
	@echo "CROSS_COMPILE is :$(CROSS_COMPILE):"
	@echo "We should run $(CROSS_COMPILE)gcc"
	@echo "CC is $(CC)"
	@echo "cross-target is $(cross-target)"

writer : writer.o

writer.o : writer.c
	$(CC) -c writer.c

# -f : won't give error if no file exists
clean :
	rm -f writer *.o

