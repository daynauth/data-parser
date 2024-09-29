CC = gcc -std=c17 -I./include
PROJECT=main
all: $(PROJECT)

json.o: ./src/json.c
	$(CC) -c ./src/json.c -o json.o

parser.o: ./src/parser.c
	$(CC) -c ./src/parser.c -o parser.o

main.o: ./src/main.c
	$(CC) -c ./src/main.c -o main.o

$(PROJECT): main.o parser.o json.o
	$(CC) main.o parser.o json.o -o $(PROJECT)

clean:
	rm -rf $(PROJECT) main.o parser.o