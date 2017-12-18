CC=gcc -Wall -Wextra -std=gnu11 -D_GNU_SOURCE -O3 -g
LIBS= -lpthread -lm 
SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o)
BINS=$(SRCS:%.c=%)

ioc: main.o functions.o
	$(CC) $^ -o $@ $(LIBS)

main.o: main.c
	$(CC) $< -c

functions.o: functions.c functions.h
	$(CC) $< -c

run: ioc
	./ioc

clean:
	rm -rf $(BINS) $(OBJS) ioc
