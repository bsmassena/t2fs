#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./include/t2fs.h"
#include "./include/apidisk.h"
#include "./include/filesystem.h"

int main() {
	int handle = 0, read = 0, write = 0;
	char buffer[10000];

	printf("\n==========================================\n");

	initialize_file_system();

	handle = open2("/file1.txt");

	read = read2(handle, buffer, 1094);
	buffer[read] = '\0';
	printf("\n%d lidos: %s", read, buffer);

	seek2(handle, 0);

	read = read2(handle, buffer, 1094);
	buffer[read] = '\0';
	printf("\n%d lidos: %s", read, buffer);

	seek2(handle, -1);

	write = write2(handle, "funfo", 5);

	seek2(handle, 0);

	read = read2(handle, buffer, 1094);
	buffer[read] = '\0';
	printf("\n%d lidos: %s", read, buffer);

	close2(handle);
	// handle = open2("/file1.txt");
	// int i;
	// for(i = 0; i < 15; i++) {
	// 	write = write2(handle, "Pica ", 5);
	// }
	// close2(handle);

	// handle = open2("/file1.txt");
	// read = read2(handle, buffer, 1094);
	// buffer[read] = '\0';
	// printf("\n%d lidos: %s", read, buffer);
	// close2(handle);

	print_file_system();
	print_fat();

	printf("\n==========================================\n");
	return 0;
}