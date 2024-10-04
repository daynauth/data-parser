CFLAGS = -Wall -Wextra -std=c2x -g
CC = clang  

SOURCE_DIR = ./src
BUILD_DIR = ./build

C_SOURCES = $(wildcard $(SOURCE_DIR)/*.c)
C_OBJECTS = $(patsubst $(SOURCE_DIR)/%.c, $(BUILD_DIR)/%.o, $(C_SOURCES))

COMPILE = $(CC) $(CFLAGS) -I./include

PROJECT=main
all: $(PROJECT)

$(BUILD_DIR)/%.o: $(SOURCE_DIR)/%.c
	$(COMPILE) -c $< -o $@

$(PROJECT): $(C_OBJECTS)
	$(COMPILE) $(C_OBJECTS) -o $(PROJECT)

clean:
	rm -rf $(PROJECT) $(C_OBJECTS)