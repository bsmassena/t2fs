#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./include/t2fs.h"
#include "./include/apidisk.h"
#include "./include/filesystem.h"

char buffer[10000];

void print_file(int handle) {
	int bytes_read = 0;
	bytes_read = read2(handle, buffer, 1000);
	buffer[bytes_read] = '\0';
	printf("%d bytes read: %s\n", bytes_read, buffer);
}

int main() {
	int handle = 0, write = 0;

	printf("\n==========================================\n");

	initialize_file_system();

	printf("Opening file '/file1.txt'\n");
	handle = open2("/file1.txt");
	printf("Returned handle: %d\n", handle);

	print_file(handle);

	printf("Seeking to 55\n");
	seek2(handle, 55);

	printf("Truncating file\n");
	truncate2(handle);

	print_file(handle);

	printf("Seeking back to 0\n");
	seek2(handle, 0);
	print_file(handle);

	printf("Closing file with handle %d\n", handle);
	close2(handle);

	// seek2(handle, 0);

	// read = read2(handle, buffer, 1094);
	// buffer[read] = '\0';
	// printf("\n%d lidos: %s", read, buffer);

	// seek2(handle, -1);

	// write = write2(handle, "funfo", 5);

	// seek2(handle, 0);

	// read = read2(handle, buffer, 1094);
	// buffer[read] = '\0';
	// printf("\n%d lidos: %s", read, buffer);

	// close2(handle);

	//print_file_system();
	//print_fat();

	printf("\n==========================================\n");
	return 0;
}