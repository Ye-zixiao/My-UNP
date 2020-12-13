CC := gcc
ROOT := ../..
INCLUDE := $(ROOT)/include
LIBPATH := $(ROOT)/lib

DLFLAGS := -Wl,-rpath $(LIBPATH) -L$(LIBPATH) -lmyunp
CFLAGS := -Wall -O -I$(INCLUDE)

SRCS := $(wildcard *.c)
OBJS := $(SRCS:c=o)
MAIN := tcpsrv_prefork4


.PHONY:
all: LIBMYUNP $(MAIN)

LIBMYUNP:
	cd $(LIBPATH) && make -j4

$(MAIN):$(OBJS)
	$(CC) -o $(MAIN) $(OBJS) $(DLFLAGS) $(CFLAGS)
	
$(OBJS):%.o:%.c

clean: rmobj
	-rm $(MAIN)

rmobj:
	-rm $(OBJS)
