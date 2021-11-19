all: schedule

statserver: schedule.c
	gcc -Wall -o schedule schedule.c

clean:
	rm -fr *~ schedule

