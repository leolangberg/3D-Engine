#main: main.o matrix.o vector.o
#	@mkdir -p bin
#	gcc build/main.o build/matrix.o build/vector.o -o bin/main -l m

#main.o: src/startup/main.c src/matrix.h
#	@mkdir -p build
#	gcc -c src/startup/main.c -o build/main.o
#
#matrix.o: src/matrix.c src/matrix.h
#	@mkdir -p build
#	gcc -c src/matrix.c -o build/matrix.o
#
#vector.o: src/vector.c src/vector.h
#	@mkdir -p build
#	gcc -c src/vector.c -o build/vector.o



SRC_DIR = src
BUILD_DIR = build
CC = gcc
SRC_FILES := $(wildcard $(SRC_DIR)/*.c) \
			 $(wildcard $(SRC_DIR)/startup/*.c) \
			 $(wildcard $(SRC_DIR)/controller/*.c) \
			 $(wildcard $(SRC_DIR)/integration/*.c) \
			 $(wildcard $(SRC_DIR)/model/*.c)
OBJ_NAME = main
INCLUDE_PATHS = -Iinclude
LIBRARY_PATHS = -Llib
COMPILER_FLAGS = -std=c11 -Wall -O0 -g
LINKER_FLAGS = -lsdl2 -lsdl2_image -lsdl2_gfx

all: $(BUILD_DIR)
	$(CC) $(COMPILER_FLAGS) $(INCLUDE_PATHS) $(SRC_FILES) $(LIBRARY_PATHS) $(LINKER_FLAGS) -o $(BUILD_DIR)/$(OBJ_NAME)

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

clean:
	rm -rf $(BUILD_DIR)/$(OBJ_NAME)
