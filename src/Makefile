CFLAGS=-g -Wall
LIBNAME=libcdbc.a
SRC=cdbc.c str.c
OBJ= $(SRC:.c=.o)

all: $(LIBNAME) main

main: main.o $(LIBNAME)
	gcc $(CFLAGS) -o main main.o $(LIBNAME) -lcurl -ljansson

cdbc.o: cdbc.c cdbc.h str.h
main.o: main.c cdbc.h

$(LIBNAME): $(OBJ)
	ar rcs $(LIBNAME) $(OBJ)

clean:
	rm -f *.o
