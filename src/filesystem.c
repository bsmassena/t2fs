#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/t2fs.h"
#include "../include/apidisk.h"
#include "../include/filesystem.h"

#define MINIMUM_FILENAME_SIZE 0
#define MAXIMUM_FILENAME_SIZE 54
#define DIR_DIVISOR '/'


// struct t2fs_superbloco {
// 	char    id[4];          	/* Identificação do sistema de arquivo. É formado pelas letras “T2FS”. */
// 	WORD    version;        	/* Versão atual desse sistema de arquivos: (valor fixo 0x7E2=2018; 2=2º semestre). */
// 	WORD    superblockSize; 	/* Quantidade de setores lógicos que formam o superbloco. (fixo em 1 setor) */
// 	DWORD	DiskSize;			/* Tamanho total, em bytes, da partição T2FS. Inclui o superbloco, a área de FAT e os clusters de dados. */
// 	DWORD	NofSectors;			/* Quantidade total de setores lógicos da partição T2FS. Inclui o superbloco, a área de FAT e os clusters de dados. */
// 	DWORD	SectorsPerCluster;	/* Número de setores lógicos que formam um cluster. */
// 	DWORD	pFATSectorStart;	/* Número do setor lógico onde a FAT inicia. */
// 	DWORD	RootDirCluster;		/* Cluster onde inicia o arquivo correspon-dente ao diretório raiz */
// 	DWORD	DataSectorStart;	/* Primeiro setor lógico da área de blocos de dados (cluster 0). */
// };

// struct t2fs_record {
// 	BYTE    TypeVal;        /* Tipo da entrada. Indica se o registro é inválido (TYPEVAL_INVALIDO), arquivo (TYPEVAL_REGULAR), diretório (TYPEVAL_DIRETORIO) ou link (TYPEVAL_LINK) */
// 	char    name[51];        Nome do arquivo. : string com caracteres ASCII (0x21 até 0x7A), case sensitive. 
// 	DWORD	bytesFileSize;	/* Tamanho do arquivo. Expresso em número de bytes. */
// 	DWORD	clustersFileSize;	/* Tamanho do arquivo. Expresso em número de clusters. */
// 	DWORD	firstCluster;	/* Número do primeiro cluster de dados correspondente a essa entrada de diretório */
// };

struct t2fs_superbloco super;


void initialize_file_system() {
	unsigned char buffer[SECTOR_SIZE];

	read_sector (0, buffer);
	memcpy(&super, buffer, sizeof(super));

	// =========== Remover para a entrega ==============
	print_super_block();
	print_file_system();
}

void read_cluster(int cluster, unsigned char *buffer) {
	int initial_sector = super.DataSectorStart + cluster * super.SectorsPerCluster;
	int i;

	for(i = 0; i < super.SectorsPerCluster; i++) {
		read_sector (initial_sector+i, &buffer[i*SECTOR_SIZE]);
	}
}




// ================ Remover as funções abaixo para a entrega ===================
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

		if(!record.TypeVal) break;

		for(j = 0; j < level; j++) {
			printf("\t");
		}

		printf("%s  %u bytes (%u clusters)\n", record.name, record.bytesFileSize, record.clustersFileSize);

		if(record.TypeVal == 0x02 && i > 1)
			print_folder(record.firstCluster, level + 1);

		// printf("\n\nType: %02X\n", record.TypeVal);
		// printf("Name: %s\n", record.name);
		// printf("File size: %u bytes\n", record.bytesFileSize);
		// printf("File size: %u clusters\n", record.clustersFileSize);
		// printf("First cluster: %u\n", record.firstCluster);
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
	printf("First data sector: %u\n\n", super.DataSectorStart);
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
