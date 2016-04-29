CC=gcc
CFLAGS= -I. -lwiringPi -Wall

all: st7920_spi_test.c
	${CC} ${CFLAGS} -o st7920_spi_test st7920_spi_test.c

clean:
	rm -f st7920_spi_test
