#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/t2fs.h"
#include "../include/apidisk.h"
#include "../include/filesystem.h"

int main() {
	int handle = 0, read = 0, write = 0;
	char buffer[10000] = "Devo ter 18 bytes.";

	printf("\n==========================================\n");

	handle = create2("teste.txt");
	if(handle < 0) {
		printf("Erro ao criar\n");
		exit(1);
	}
	printf("Handle: %d\n", handle);

	print_fat();
	print_file_system();

	read = read2(handle, buffer, 10000);
	printf("Read: %d\n", read);
	printf("%s\n", buffer);

	write = write2(handle, buffer, strlen(buffer));
	printf("Write: %d\n", write);
	seek2(handle, 0);

	read = read2(handle, buffer, 10000);
	buffer[read] = '\0';
	printf("Read: %d\n", read);
	//printf("%s\n", buffer);
	
	seek2(handle, 0);

	memset(buffer, '*', 5000);

	write = write2(handle, buffer, 5000);
	printf("Write: %d\n", write);

	print_fat();
	print_file_system();

	seek2(handle, 0);

	write = write2(handle, buffer, 4000);
	printf("Write: %d\n", write);
	seek2(handle, 0);

	read = read2(handle, buffer, 100000);
	buffer[read] = '\0';
	printf("Read: %d\n", read);
	//printf("%s\n", buffer);

	print_fat();
	print_file_system();

	printf("\n==========================================\n");
	return 0;
}
