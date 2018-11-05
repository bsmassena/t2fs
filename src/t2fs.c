#include <stdio.h>
#include <string.h>
#include "../include/apidisk.h"
#include "../include/t2fs.h"
#include "../include/filesystem.h"

int identify2 (char *name, int size){
	char names[] = "Bruno Santana Massena de Lima - 274711\nPedro Salgado Perrone - 274696";
	if(size < strlen(names)) {
		printf("ERROR: the given size for the names is not big enough.\n");
		return -1;
	}
	strncpy (name, names, size);
	return 0;
}

FILE2 create2 (char *filename){
	initialize_file_system();
	return create_file(filename, TYPEVAL_REGULAR);
}

int delete2 (char *filename){
	initialize_file_system();
	return delete_file(filename);
}

FILE2 open2 (char *filename){
	initialize_file_system();
	return open_file(filename);
}

int close2 (FILE2 handle){
	initialize_file_system();
	return close_file(handle);
}

int read2 (FILE2 handle, char *buffer, int size){
	initialize_file_system();
	return read_file(handle, buffer, size);
}

int write2 (FILE2 handle, char *buffer, int size){
	initialize_file_system();
	return write_file(handle, buffer, size);
}

int truncate2 (FILE2 handle){
	initialize_file_system();
	return truncate_file(handle);
}

int seek2 (FILE2 handle, DWORD offset){
	initialize_file_system();
	return seek_file(handle, offset);
}

int mkdir2 (char *pathname){
	initialize_file_system();
	return make_dir(pathname);
}

int rmdir2 (char *pathname){
	initialize_file_system();
	return remove_dir(pathname);
}

int chdir2 (char *pathname){
	initialize_file_system();
	return change_dir(pathname);
}

int getcwd2 (char *pathname, int size){
	initialize_file_system();
	return get_current_work_directory(pathname, size);
}

DIR2 opendir2 (char *pathname){
	return (0);
}

int readdir2 (DIR2 handle, DIRENT2 *dentry){
	return (0);
}

int closedir2 (DIR2 handle){
	return (0);
}

int ln2(char *linkname, char *filename){
	initialize_file_system();
	return create_link(linkname, filename);
}
