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
