#ifndef __filesystem__
#define __filesystem__

#include "t2fs.h"

typedef struct t2fs_superbloco Super;
typedef struct t2fs_record Record;

typedef struct {
    int 	handle;
    int 	curr_pointer;
    Record 	file;
    Record 	parent;
} Descriptor;


#define	FAT_EMPTY 0
#define	FAT_END -1
#define	FAT_INVALID -2

#define RECORDS_PER_SECTOR 4

#define DESCRIPTORS_SIZE 20


// Initialize functions
void initialize_file_system();
void read_super_block();
void read_fat();
void read_root_dir();

// File functions
int create_file(char *filename);
int delete_file(char *filename);
int open_file(char *filename);
int close_file(FILE2 handle);
int read_file(FILE2 handle, char *buffer, int size);
int write_file(FILE2 handle, char *buffer, int size);
int truncate_file(FILE2 handle);
int seek_file(FILE2 handle, DWORD offset);

// Record functions
void init_record(Record *record, int type, char *filename, int cluster);
int read_record(int cluster, int index, Record *record);
int write_record(int cluster, Record *record);
int remove_record(int cluster, char *record_name);
int search_record(int cluster, char *record_name, Record *record);

// FAT functions
void save_fat();
int remove_fat_sequence(int start_index);
int increase_fat_sequence(int initial_cluster, int clusters);

// Cluster functions
void read_cluster(int cluster, unsigned char *buffer);
void write_cluster(int cluster, unsigned char *buffer);
void clear_cluster(int cluster);
int find_free_cluster();
int get_cluster_start_sector(int cluster);

// Descriptors functions
void clear_descriptors();
int create_global_descriptor(Descriptor desc);
int descriptor_from_path(Descriptor *descriptor, char *filename);
int remove_handle(FILE2 handle);

// Debug functions
void print_file_system();
void print_folder(int cluster, int level);
void print_super_block();
void print_record(Record record);
void print_fat();

#endif