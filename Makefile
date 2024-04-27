CC = gcc

# $(CC) -o bmp_write.out bmp_write.c
# ./bmp_write.out
# rm bmp_write.out

all:
	$(CC) -o main.out main.c bmp.h bmp.c

run:
	./main.out
clean:
	rm main.out
show:
	feh ./my_bmp.bmp
