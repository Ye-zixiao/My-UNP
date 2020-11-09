CC := gcc
DFLAGS := -lunp
CFLAGS := -O -Wall
SRCS := $(wildcard *.c)
OBJS := $(SRCS:c=o)
PRGS := $(SRCS:.c=)

.PHONY:
all: $(PRGS)

$(PRGS):%:%.o
	$(CC) -o $@ $< $(CFLAGS) $(DFLAGS)

$(OBJS):%.o:%.c

clean: rmobj
	-rm $(PRGS)

rmobj:
	-rm $(OBJS)
