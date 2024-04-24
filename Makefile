CC = gcc

all:
	$(CC) -o bmp_write.out bmp_write.c

run:
	./bmp_write.out

clean:
	rm bmp_write.out


