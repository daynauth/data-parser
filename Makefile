CC = gcc -std=c17 -I./include/json.h
PROJECT=main
all: $(PROJECT)

parser.o: ./src/parser.c
	$(CC) -c ./src/parser.c -o parser.o

main.o: ./src/main.c
	$(CC) -c ./src/main.c -o main.o

$(PROJECT): main.o parser.o
	$(CC) main.o parser.o -o $(PROJECT)

clean:
	rm -rf $(PROJECT) main.o parser.o