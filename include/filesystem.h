#ifndef __filesystem__
#define __filesystem__

#include "t2fs.h"

void initialize_file_system();
void read_cluster(int cluster, unsigned char *buffer);
DWORD little_to_big(DWORD little);
void print_file_system();
void print_folder(int cluster, int level);
void print_super_block();

#endif