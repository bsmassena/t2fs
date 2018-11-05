#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/t2fs.h"
#include "../include/apidisk.h"
#include "../include/filesystem.h"

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

	// print_fat();
	// print_file_system();

	printf("\n%d\n", chdir2("/dir1/file1.txt"));

	printf("\n==========================================\n");
	return 0;
}
