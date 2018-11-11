CC=gcc
LIB_DIR=../lib
FLAGS=-L$(LIB_DIR) -lt2fs -lm -Wall

all: teste name_validator getcwd link read_write dir_functions

teste: teste.o $(LIB_DIR)/libt2fs.a
	$(CC) -o teste teste.o $(FLAGS)
teste.o: teste.c
	$(CC) -c teste.c -Wall


name_validator: name_validator.o $(LIB_DIR)/libt2fs.a
	$(CC) -o name_validator name_validator.o $(FLAGS)
name_validator.o: name_validator.c
	$(CC) -c name_validator.c -Wall


getcwd: getcwd2.o $(LIB_DIR)/libt2fs.a
	$(CC) -o getcwd getcwd2.o $(FLAGS)
getcwd2.o: getcwd2.c
	$(CC) -c getcwd2.c -Wall


link: link.o $(LIB_DIR)/libt2fs.a
	$(CC) -o link link.o $(FLAGS)
link.o: link.c
	$(CC) -c link.c -Wall


read_write: read_write.o $(LIB_DIR)/libt2fs.a
	$(CC) -o read_write read_write.o $(FLAGS)
read_write.o: read_write.c
	$(CC) -c read_write.c -Wall


dir_functions: dir_functions.o $(LIB_DIR)/libt2fs.a
	$(CC) -o dir_functions dir_functions.o $(FLAGS)
dir_functions.o: dir_functions.c
	$(CC) -c dir_functions.c -Wall

clean:
	rm *.o teste name_validator getcwd link dir_functions read_write
