#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/t2fs.h"
#include "../include/apidisk.h"
#include "../include/filesystem.h"

void print_dentry(DIRENT2 dentry) {
	printf("Name: %s\n", dentry.name);
	printf("Type: %d\n", dentry.fileType);
	printf("Size: %d\n\n", dentry.fileSize);
}

int main() {
	int handle = 0;
	DIRENT2 dentry;

	handle = opendir2(".");
	if(handle <= 0) {
		printf("Erro, handle = %d\n", handle);
		exit(1);
	} else {
		printf("Handle: %d\n", handle);
	}

	while(1)
		if(readdir2(handle, &dentry) < 0)
			break;
		else
			print_dentry(dentry);

	printf("Fechando handle %d: %d\n", handle, closedir2(handle));
	return 0;
}
