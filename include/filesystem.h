#ifndef __filesystem__
#define __filesystem__

#include "t2fs.h"

typedef struct t2fs_superbloco Super;
typedef struct t2fs_record Record;

void initialize_file_system();
void read_cluster(int cluster, unsigned char *buffer);
void read_super_block();
void read_fat();
void read_root_dir();
int get_cluster_start_sector(int cluster);
void print_file_system();
void print_folder(int cluster, int level);
void print_super_block();
void print_record(Record record);

#endif