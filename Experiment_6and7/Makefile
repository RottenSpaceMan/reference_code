CC = gcc
CFLAGS = -Wall -Wextra -pthread
ITEMS = SRARQreceiver SRARQsender

all:
	$(CC) $(CFLAGS) SRARQreceiver.c -o SRARQreceiver
	$(CC) $(CFLAGS) SRARQsender.c -o SRARQsender

clean:
	rm -f $(ITEMS)
