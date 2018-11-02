#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "../include/t2fs.h"
#include "../include/apidisk.h"
#include "../include/filesystem.h"

#define MINIMUM_FILENAME_SIZE 0
#define MAXIMUM_FILENAME_SIZE 54
#define DIR_DIVISOR '/'


// struct t2fs_superbloco {
//  char    id[4];              /* Identificação do sistema de arquivo. É formado pelas letras “T2FS”. */
//  WORD    version;            /* Versão atual desse sistema de arquivos: (valor fixo 0x7E2=2018; 2=2º semestre). */
//  WORD    superblockSize;     /* Quantidade de setores lógicos que formam o superbloco. (fixo em 1 setor) */
//  DWORD   DiskSize;           /* Tamanho total, em bytes, da partição T2FS. Inclui o superbloco, a área de FAT e os clusters de dados. */
//  DWORD   NofSectors;         /* Quantidade total de setores lógicos da partição T2FS. Inclui o superbloco, a área de FAT e os clusters de dados. */
//  DWORD   SectorsPerCluster;  /* Número de setores lógicos que formam um cluster. */
//  DWORD   pFATSectorStart;    /* Número do setor lógico onde a FAT inicia. */
//  DWORD   RootDirCluster;     /* Cluster onde inicia o arquivo correspon-dente ao diretório raiz */
//  DWORD   DataSectorStart;    /* Primeiro setor lógico da área de blocos de dados (cluster 0). */
// };

// struct t2fs_record {
//  BYTE    TypeVal;        /* Tipo da entrada. Indica se o registro é inválido (TYPEVAL_INVALIDO), arquivo (TYPEVAL_REGULAR), diretório (TYPEVAL_DIRETORIO) ou link (TYPEVAL_LINK) */
//  char    name[51];        Nome do arquivo. : string com caracteres ASCII (0x21 até 0x7A), case sensitive. 
//  DWORD   bytesFileSize;  /* Tamanho do arquivo. Expresso em número de bytes. */
//  DWORD   clustersFileSize;   /* Tamanho do arquivo. Expresso em número de clusters. */
//  DWORD   firstCluster;   /* Número do primeiro cluster de dados correspondente a essa entrada de diretório */
// };

Super super;
Record root_dir;
Record curr_dir;
Descriptor descriptors[DESCRIPTORS_SIZE];
DWORD *fat;

int fat_entries = 0;
int records_per_cluster = 0;

int initialized = 0;


void initialize_file_system() {
    if(initialized) return;
    read_super_block();
    read_fat();
    read_root_dir();
    clear_descriptors();

    initialized = 1;
    records_per_cluster = RECORDS_PER_SECTOR * super.SectorsPerCluster;
    curr_dir = root_dir;

    // =========== Remover para a entrega ==============
    // print_super_block();
    // print_file_system();
    // print_record(root_dir);
    // print_fat();
}

void read_super_block() {
    unsigned char buffer[SECTOR_SIZE];

    read_sector (0, buffer);
    memcpy(&super, buffer, sizeof(super));
}

void read_fat() {
    int fat_start = super.pFATSectorStart;
    int fat_end = super.DataSectorStart;
    int fat_sectors = fat_end - fat_start;
    int fat_size = SECTOR_SIZE * fat_sectors;
    unsigned char sec_buffer[SECTOR_SIZE];
    int i, start_index;

    fat = malloc(fat_size);

    for(i = fat_start; i < fat_end; i++) {
        start_index = (i - fat_start) * SECTOR_SIZE / 4;
        read_sector(i, sec_buffer);
        memcpy(&fat[start_index], sec_buffer, SECTOR_SIZE);
    }

    fat_entries = fat_size / 4;
}

void read_root_dir() {
    unsigned char buffer[SECTOR_SIZE * super.SectorsPerCluster];

    read_cluster (super.RootDirCluster, buffer);
    memcpy(&root_dir, buffer, sizeof(root_dir));
}

int create_file(char *filename) {
    unsigned char buffer[SECTOR_SIZE * super.SectorsPerCluster];
    Descriptor desc;
    int file_cluster = find_free_cluster();
    int handle;

    // Return if the path isn't valid
    if(descriptor_from_path(&desc, filename) < 0) return -1;

    desc.file.TypeVal = TYPEVAL_REGULAR;
    desc.file.bytesFileSize = 0;
    desc.file.clustersFileSize = 1;
    desc.file.firstCluster = file_cluster;

    // Return if can't create a record
    if(write_record(desc.parent.firstCluster, &desc.file) < 0) return -1;

    handle = create_global_descriptor(desc);

    // Return if can't create a handle
    if(handle < 0) return -1;

    // Write EOF at file
    buffer[0] = EOF;
    write_cluster(file_cluster, buffer);

    // Update FAT
    fat[file_cluster] = FAT_END;
    save_fat();

    return handle;
}

int delete_file(char *filename) {
    Descriptor desc;

    // Return if the path isn't valid
    if(descriptor_from_path(&desc, filename) < 0) return -1;
    if(desc.file.TypeVal != TYPEVAL_REGULAR &&
       desc.file.TypeVal != TYPEVAL_LINK) return -1;

    remove_record(desc.parent.firstCluster, desc.file.name);

    // Return if can't remove fat entries
    if(remove_fat_sequence(desc.file.firstCluster) < 0) return -1;

    save_fat();

    return 0;
}

int open_file (char *filename) {
    Descriptor desc;
    int handle;

    // Return if the path isn't valid
    if(descriptor_from_path(&desc, filename) < 0) return -1;
    if(desc.file.TypeVal != TYPEVAL_REGULAR &&
       desc.file.TypeVal != TYPEVAL_LINK) return -1;

    handle = create_global_descriptor(desc);

    // Return if can't create a handle
    if(handle < 0) return -1;

    return handle;
}

int close_file (FILE2 handle) {
    return remove_handle(handle);
}

int read_file (FILE2 handle, char *buffer, int size) {
    Descriptor desc = descriptors[handle - 1];
    int bytes_left, cluster_to_read, i;

    unsigned char file[desc.file.clustersFileSize * SECTOR_SIZE * super.SectorsPerCluster];
    bytes_left = desc.file.bytesFileSize - desc.curr_pointer;
    cluster_to_read = desc.file.firstCluster;

    // Read entire file
    for(i = 0; i < desc.file.clustersFileSize; i++) {
        read_cluster(cluster_to_read, &file[i * SECTOR_SIZE * super.SectorsPerCluster]);
        cluster_to_read = fat[cluster_to_read];
    }

    if(bytes_left < size) {
        size = bytes_left;
    }

    memcpy(buffer, &file[desc.curr_pointer], size);
    desc.curr_pointer += size;
    descriptors[handle - 1] = desc;

    return size;
}

int write_file(FILE2 handle, char *buffer, int size) {
    Descriptor desc = descriptors[handle - 1];
    int cluster, i, max_size_after_write, clusters_needed, clusters_allocated;

    max_size_after_write = (desc.file.clustersFileSize * SECTOR_SIZE * super.SectorsPerCluster) + size;

    unsigned char file[max_size_after_write];
    cluster = desc.file.firstCluster;

    // Read entire file into a buffer
    for(i = 0; i < desc.file.clustersFileSize; i++) {
        read_cluster(cluster, &file[i * SECTOR_SIZE * super.SectorsPerCluster]);
        cluster = fat[cluster];
    }

    // Change the file
    memcpy(&file[desc.curr_pointer], buffer, size);
    
    cluster = desc.file.firstCluster;

    if(desc.curr_pointer + size > desc.file.bytesFileSize) {
        clusters_needed = ceil((float)(desc.curr_pointer + size) / (SECTOR_SIZE * super.SectorsPerCluster)) - desc.file.clustersFileSize;
        desc.file.bytesFileSize = desc.curr_pointer + size;
    } else {
        clusters_needed = ceil((float)(desc.file.bytesFileSize + size) / (SECTOR_SIZE * super.SectorsPerCluster)) - desc.file.clustersFileSize;
    }

    clusters_allocated = increase_fat_sequence(cluster, clusters_needed) + desc.file.clustersFileSize;

    file[desc.file.bytesFileSize] = EOF;

    // Write entire file back to disk
    for(i = 0; i < clusters_allocated; i++) {
        write_cluster(cluster, &file[i * SECTOR_SIZE * super.SectorsPerCluster]);
        cluster = fat[cluster];
    }

    desc.file.clustersFileSize = clusters_allocated;

    remove_record(desc.parent.firstCluster, desc.file.name);
    write_record(desc.parent.firstCluster, &desc.file);

    desc.curr_pointer += size;
    descriptors[handle - 1] = desc;

    return size;
}

int truncate_file (FILE2 handle) {
    Descriptor desc = descriptors[handle - 1];
    int i, truncate_cluster, cluster;

    truncate_cluster = ceil((float) desc.curr_pointer / (SECTOR_SIZE * super.SectorsPerCluster));

    unsigned char file[desc.file.clustersFileSize * SECTOR_SIZE * super.SectorsPerCluster];
    cluster = desc.file.firstCluster;

    // Read entire file into a buffer
    for(i = 0; i < desc.file.clustersFileSize; i++) {
        read_cluster(cluster, &file[i * SECTOR_SIZE * super.SectorsPerCluster]);
        cluster = fat[cluster];
    }

    file[desc.curr_pointer] = EOF;
    cluster = desc.file.firstCluster;

    // Write entire file back to disk
    for(i = 0; i < desc.file.clustersFileSize; i++) {
        write_cluster(cluster, &file[i * SECTOR_SIZE * super.SectorsPerCluster]);
        cluster = fat[cluster];
    }

    cluster = desc.file.firstCluster;

    // Remove unused FAT entries
    for(i = 0; i < truncate_cluster; i++) {
        cluster = fat[cluster];
    }

    remove_fat_sequence(fat[cluster]);
    fat[cluster] = FAT_END;

    desc.file.bytesFileSize = desc.curr_pointer;
    desc.file.clustersFileSize = truncate_cluster;

    remove_record(desc.parent.firstCluster, desc.file.name);
    write_record(desc.parent.firstCluster, &desc.file);

    descriptors[handle - 1] = desc;

    return 0;
}

int seek_file(FILE2 handle, DWORD offset) {
    Descriptor desc = descriptors[handle - 1];

    if(offset == -1) {
        desc.curr_pointer = desc.file.bytesFileSize;
    } else {
        desc.curr_pointer = offset;
    }

    descriptors[handle - 1] = desc;

    return 0;
}

// Dir functions
int make_dir (char *pathname) {
    Descriptor desc;
    int dir_cluster = find_free_cluster();

    // Return if the path isn't valid
    if(descriptor_from_path(&desc, pathname) < 0) return -1;

    desc.file.TypeVal = TYPEVAL_DIRETORIO;
    desc.file.bytesFileSize = 1024;
    desc.file.clustersFileSize = 1;
    desc.file.firstCluster = dir_cluster;

    // Return if can't create a record
    if(write_record(desc.parent.firstCluster, &desc.file) < 0) return -1;

    // Create . and ..
    initialize_dir_records(desc);

    // Update FAT
    fat[dir_cluster] = FAT_END;
    save_fat();

    return 0;
}

int read_record(int cluster, int index, Record *record) {
    unsigned char buffer[SECTOR_SIZE * super.SectorsPerCluster];

    read_cluster(cluster, buffer);
    memcpy(record, &buffer[index * RECORD_SIZE], sizeof(Record));

    if(record->TypeVal == TYPEVAL_INVALIDO) {
        return -1;
    }

    return 0;
}

int write_record(int cluster, Record *record) {
    unsigned char buffer[SECTOR_SIZE * super.SectorsPerCluster];
    Record aux_record;
    int i;

    read_cluster(cluster, buffer);

    for(i = 0; i < records_per_cluster; i++) {
        memcpy(&aux_record, &buffer[i * RECORD_SIZE], sizeof(Record));

        // If a file with this name already exists
        if(aux_record.TypeVal != TYPEVAL_INVALIDO && strcmp(aux_record.name, record->name) == 0) {
            return -1;
        }

        if(aux_record.TypeVal == TYPEVAL_INVALIDO) {
            memcpy(&buffer[i * RECORD_SIZE], record, sizeof(Record));
            write_cluster(cluster, buffer);
            return 0;
        }
    }

    // Directory is full
    return -1;
}

int remove_record(int cluster, char *record_name) {
    unsigned char buffer[SECTOR_SIZE * super.SectorsPerCluster];
    int i;
    Record record;

    read_cluster(cluster, buffer);

    for(i = 0; i < records_per_cluster; i++) {
        memcpy(&record, &buffer[i * RECORD_SIZE], sizeof(Record));
        if(strcmp(record.name, record_name) == 0) {
            record.TypeVal = TYPEVAL_INVALIDO;

            memcpy(&buffer[i * RECORD_SIZE], &record, sizeof(Record));
            write_cluster(cluster, buffer);
            return i;
        }
    }
    return -1;
}

int search_record(int cluster, char *record_name, Record *record) {
    int i;

    for(i = 0; i < records_per_cluster; i++) {
        if(read_record(cluster, i, record) == 0 &&
           strcmp(record->name, record_name) == 0) {
            return i;
        }
    }
    return -1;
}

void initialize_dir_records(Descriptor desc){
    Record self, parent;

    clear_cluster(desc.file.firstCluster);

    self.TypeVal = TYPEVAL_DIRETORIO;
    strcpy(self.name, ".");
    self.bytesFileSize = 1024;
    self.clustersFileSize = 1;
    self.firstCluster = desc.file.firstCluster;

    write_record(desc.file.firstCluster, &self);

    parent.TypeVal = TYPEVAL_DIRETORIO;
    strcpy(parent.name, "..");
    parent.bytesFileSize = 1024;
    parent.clustersFileSize = 1;
    parent.firstCluster = desc.parent.firstCluster;

    write_record(desc.file.firstCluster, &parent);
}


void save_fat() {
    int fat_start = super.pFATSectorStart;
    int fat_end = super.DataSectorStart;
    int i, start_index;

    for(i = fat_start; i < fat_end; i++) {
        start_index = (i - fat_start) * SECTOR_SIZE / 4;
        write_sector(i, &fat[start_index]);
    }
}

int remove_fat_sequence(int start_index) {
    if(fat[start_index] == FAT_EMPTY ||
       fat[start_index] == FAT_INVALID) {
        return -1;
    }

    if(fat[start_index] == FAT_END) {
        fat[start_index] = 0;
        return 0;
    }

    if(remove_fat_sequence(fat[start_index]) == 0) {
        fat[start_index] = 0;
        return 0;
    }

    return -1;
}

int increase_fat_sequence(int initial_cluster, int clusters) {
    int i, new_cluster;

    if(clusters == 0) return 0;

    while(fat[initial_cluster] != FAT_END) {
        initial_cluster = fat[initial_cluster];
    }

    for(i = 0; i < clusters; i++) {
        new_cluster = find_free_cluster();
        if(new_cluster < 0) {
            fat[initial_cluster] = FAT_END;
            return i;
        }
        fat[initial_cluster] = new_cluster;
        initial_cluster = new_cluster;
    }

    fat[initial_cluster] = FAT_END;
    return clusters;
}

void read_cluster(int cluster, unsigned char *buffer) {
    int initial_sector = get_cluster_start_sector(cluster);
    int i;

    for(i = 0; i < super.SectorsPerCluster; i++) {
        read_sector (initial_sector + i, &buffer[i*SECTOR_SIZE]);
    }
}

void write_cluster(int cluster, unsigned char *buffer) {
    int initial_sector = get_cluster_start_sector(cluster);
    int i;

    for(i = 0; i < super.SectorsPerCluster; i++) {
        write_sector (initial_sector + i, &buffer[i*SECTOR_SIZE]);
    }
}

void clear_cluster(int cluster) {
    unsigned char buffer[SECTOR_SIZE * super.SectorsPerCluster];
    int i;

    for(i = 0; i < SECTOR_SIZE * super.SectorsPerCluster; i++) {
        buffer[i] = '\0';
    }

    write_cluster(cluster, buffer);
}

int find_free_cluster() {
    int i;

    for(i = 0; i < fat_entries; i++) {
        if(fat[i] == FAT_EMPTY) return i;
    }
    return -1;
}

int get_cluster_start_sector(int cluster) {
    return (super.DataSectorStart + cluster * super.SectorsPerCluster);
}

void clear_descriptors() {
    int i;

    for(i = 0; i < DESCRIPTORS_SIZE ; i++) {
        descriptors[i].handle = 0;
        descriptors[i].curr_pointer = 0;
    }
}

int create_global_descriptor(Descriptor desc) {
    int i;

    for(i = 0; i < DESCRIPTORS_SIZE ; i++) {
        if(descriptors[i].handle == 0) {
            desc.handle = i + 1;
            desc.curr_pointer = 0;
            memcpy(&descriptors[i], &desc, sizeof(Descriptor));
            return descriptors[i].handle;
        }
    }
    return -1;
}

int descriptor_from_path(Descriptor *descriptor, char *filename) {
    char s[2] = "/";
    char *token, last_token[51];
    char path_copy[strlen(filename)];

    // Return if the given path isn't valid
    // if(!path_is_valid(filename)) return -1;

    strcpy(path_copy, filename);

    // Starts from root or current_dir
    if(path_copy[0] == '/')
        memcpy(&descriptor->file, &root_dir, sizeof(Record));
    else
        memcpy(&descriptor->file, &curr_dir, sizeof(Record));

    token = strtok(path_copy, s);

    while( token != NULL ) {
        if(descriptor->file.TypeVal != TYPEVAL_DIRETORIO) return -1;
        memcpy(&descriptor->parent, &descriptor->file, sizeof(Record));

        if(search_record(descriptor->parent.firstCluster, token, &descriptor->file) < 0) {
            strcpy(last_token, token);
            token = strtok(NULL, s);
            if(token == NULL) {
                descriptor->file.TypeVal = TYPEVAL_INVALIDO;
                strcpy(descriptor->file.name, last_token);
                return 0;
            } else
                return -1;
        }
        token = strtok(NULL, s);
    }

    return 0;
}

int remove_handle(FILE2 handle) {
    int i;

    for(i = 0; i < DESCRIPTORS_SIZE ; i++) {
        if(descriptors[i].handle == handle) {
            descriptors[i].handle = 0;
            return 0;
        }
    }
    return -1;
}

int file_name_is_valid(char file_name[]) {
    int i = 0;
    if(!length_is_valid(file_name)) return 0;
    while(file_name[i] != '\0') {
        if(!character_is_valid(file_name[i])) {
            return 0;
        }
        i++;
    }
    return 1;
}
int length_is_valid(char file_name[]) {
    int length = strlen(file_name);
    return length >= MINIMUM_FILENAME_SIZE && length <= MAXIMUM_FILENAME_SIZE;
}
int character_is_valid(char character) {
    return is_capital_case(character) || is_lower_case(character)
            || is_number(character);
}
int is_capital_case(char character) {
    return character >= 'A' && character <= 'Z';
}
int is_lower_case(char character) {
    return character >= 'a' && character <= 'z';
}
int is_number(char character) {
    return character >= '0' && character <= '9';
}
int path_is_valid(char path[]) {
    if(strlen(path) == 0) return 0;
    if(has_two_separators_in_a_row(path)) return 0;
    if(ends_with_separator(path)) return 0;
    if(character_is_valid(path[0])) {
        return all_names_are_valid(path);
    }
    if(is_absolute_path(path)) {
        return all_names_are_valid(&path[1]);
    }
    if(starts_with_current_directory(path)) {
        return all_names_are_valid(&path[2]);
    }
    if(starts_with_parent_directory(path)) {
        return path_is_valid(&path[3]);
    }
    return 0;
}
int has_two_separators_in_a_row(char path[]) {
    int i = 0;
    while(path[i] != '\0') {
        if(path[i] == DIR_DIVISOR && path[i + 1] == DIR_DIVISOR){
            return 1;
        }
        i++;
    }
    return 0;
}
int ends_with_separator(char path[]) {
    int len = strlen(path);
    return path[len - 1] == DIR_DIVISOR;
}
int is_absolute_path(char path[]) {
    return path[0] == DIR_DIVISOR;
}
int all_names_are_valid(char normalized_path[]) {
    char s[2] = { DIR_DIVISOR, '\0' };
    char *filename;
    char path_copy[strlen(normalized_path)];
    strcpy(path_copy, normalized_path);
    filename = strtok(path_copy, s);
    while( filename != NULL ) {
        if(!file_name_is_valid(filename)) return 0;
        filename = strtok(NULL, s);
    }
    return 1;
}
int starts_with_current_directory(char path[]) {
    return path[0] == '.' && path[1] == '/';
}
int starts_with_parent_directory(char path[]) {
    return path[0] == '.' && path[1] == '.' && path[2] == DIR_DIVISOR;
}

// ================ Funções para debug ===================
void print_file_system() {
    printf("\n========== FILE SYSTEM ==========\n");
    print_folder(super.RootDirCluster, 0);
}

void print_folder(int cluster, int level) {
    int cluster_size = SECTOR_SIZE * super.SectorsPerCluster;
    unsigned char buffer[cluster_size];
    struct t2fs_record record;
    int i, j;

    read_cluster(cluster, buffer);

    for(i = 0; i < cluster_size / RECORD_SIZE; i++) {

        memcpy(&record, &buffer[i*RECORD_SIZE], sizeof(record));

        if(!record.TypeVal) continue;

        for(j = 0; j < level; j++) {
            printf("\t");
        }

        printf("%s  %u bytes (%u clusters) -  %u\n", record.name, record.bytesFileSize, record.clustersFileSize, record.firstCluster);

        if(record.TypeVal == 0x02 && i > 1)
            print_folder(record.firstCluster, level + 1);
    }
}

void print_super_block() {
    printf("\n========== SUPER BLOCK ==========\n");
    printf("Id: %.4s\n", super.id);
    printf("Version: %hu\n", super.version);
    printf("Super block size: %hu sectors\n", super.superblockSize);
    printf("Disk size: %u bytes\n", super.DiskSize);
    printf("Disk size: %u sectors\n", super.NofSectors);
    printf("Setors per cluster: %u sectors\n", super.SectorsPerCluster);
    printf("FAT start sector: %u\n", super.pFATSectorStart);
    printf("Root cluster: %u\n", super.RootDirCluster);
    printf("First data sector: %u\n", super.DataSectorStart);
}

void print_record(Record record) {
    printf("\n========== RECORD ==========\n");
    printf("Type: %02X\n", record.TypeVal);
    printf("Name: %s\n", record.name);
    printf("File size: %u bytes\n", record.bytesFileSize);
    printf("File size: %u clusters\n", record.clustersFileSize);
    printf("First cluster: %u\n", record.firstCluster);
}

void print_fat() {
    int i;

    printf("\n========== FAT (20 ENTRADAS) ==========\n");

    for(i = 0; i < 20; i++) {
        printf("%d: %02X\n", i, fat[i]);
    }
}
