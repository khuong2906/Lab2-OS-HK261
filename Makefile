CC = gcc
CFLAGS = -Wall -O2
LDFLAGS = -pthread

TARGETS = movie_average_shm sum_serial sum_multi_thread message_queue mmap_share

all: $(TARGETS)

movie_average_shm: p1/movie_average_shm.c
	$(CC) $(CFLAGS) $< -o $@

sum_serial: p2/sum_serial.c
	$(CC) $(CFLAGS) $< -o $@

sum_multi_thread: p2/sum_multi_thread.c
	$(CC) $(CFLAGS) $< -o $@ $(LDFLAGS)

message_queue: p3/message_queue.c
	$(CC) $(CFLAGS) $< -o $@

mmap_share: p4/mmap_share.c
	$(CC) $(CFLAGS) $< -o $@

p1: movie_average_shm
p2: sum_serial sum_multi_thread
p3: message_queue
p4: mmap_share

clean:
	rm -f $(TARGETS) *.o
