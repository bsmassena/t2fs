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

#define MINIMUM_FILENAME_SIZE 0
#define MAXIMUM_FILENAME_SIZE 54
#define DIR_DIVISOR '/'

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

// Dir functions
int make_dir (char *pathname);

// Record functions
int read_record(int cluster, int index, Record *record);
int write_record(int cluster, Record *record);
int remove_record(int cluster, char *record_name);
int search_record(int cluster, char *record_name, Record *record);
void initialize_dir_records(Descriptor desc);

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

// Name validation
int file_name_is_valid(char file_name[]);
int character_is_valid(char character);
int is_capital_case(char character);
int is_lower_case(char character);
int is_number(char character);
int length_is_valid(char file_name[]);
int path_is_valid(char path[]);
int has_two_separators_in_a_row(char path[]);
int ends_with_separator(char path[]);
int relative_path_is_valid(char path[]);
int is_absolute_path(char path[]);
int all_names_are_valid(char normalized_path[]);
int starts_with_current_directory(char path[]);
int starts_with_parent_directory(char path[]);

// Debug functions
void print_file_system();
void print_folder(int cluster, int level);
void print_super_block();
void print_record(Record record);
void print_fat();

#endif
