CC=gcc

all: barcherod

barcherod: barcherod.c
	   $(CC) barcherod.c -o barcherod -lpthread $$(pkg-config allegro-5 allegro_font-5 allegro_image-5 allegro_primitives-5 --libs --cflags)

clean:
	rm -rf *.o barcherod