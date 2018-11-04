CC=gcc
LIB_DIR=../lib
FLAGS=-L$(LIB_DIR) -lt2fs -lm -Wall

all: teste name_validator getcwd

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

clean:
	rm *.o teste name_validator
