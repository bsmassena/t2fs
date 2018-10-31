#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/t2fs.h"
#include "../include/apidisk.h"
#include "../include/filesystem.h"


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

Super super;
Record root_dir;
Record current_dir;
DWORD *fat;

int initialized = 0;


void initialize_file_system() {
	read_super_block();
	read_fat();
	read_root_dir();

	initialized = 1;

	// =========== Remover para a entrega ==============
	print_super_block();
	print_file_system();
	print_record(root_dir);
	print_fat();
}

void read_cluster(int cluster, unsigned char *buffer) {
	int initial_sector = get_cluster_start_sector(cluster);
	int i;

	for(i = 0; i < super.SectorsPerCluster; i++) {
		read_sector (initial_sector+i, &buffer[i*SECTOR_SIZE]);
	}
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
	int i;

	fat = malloc(fat_size);

	for(i = fat_start; i < fat_end; i++) {
		read_sector(i, sec_buffer);
		memcpy(&fat[(i - fat_start) * SECTOR_SIZE / 4], sec_buffer, SECTOR_SIZE);
	}

	//read_sector (0, buffer);
	//memcpy(&super, buffer, sizeof(super));
}

void read_root_dir() {
	unsigned char buffer[SECTOR_SIZE * super.SectorsPerCluster];

	read_cluster (super.RootDirCluster, buffer);
	memcpy(&root_dir, buffer, sizeof(root_dir));
}

int get_cluster_start_sector(int cluster) {
	return (super.DataSectorStart + cluster * super.SectorsPerCluster);
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

		if(!record.TypeVal) break;

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

	printf("\n========== FAT ==========\n");

	for(i = 0; i < 20; i++) {
		printf("%d: %02X\n", i, fat[i]);
	}
}
