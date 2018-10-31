#ifndef __filesystem__
#define __filesystem__

#include "t2fs.h"

void initialize_file_system();
void read_cluster(int cluster, unsigned char *buffer);
DWORD little_to_big(DWORD little);
void print_file_system();
void print_folder(int cluster, int level);
void print_super_block();

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

#endif
