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
struct t2fs_record record;

void initialize_file_system() {
	unsigned char buffer[SECTOR_SIZE];
	unsigned char cluster[SECTOR_SIZE*4];
	int i, ret;

	for(i = 0; i < 200; i++) {
		ret = read_sector (i, buffer);
		if(ret != 0)
			printf("Erro %d\n", i);
	}

	read_sector (0, buffer);
	memcpy(&super, buffer, sizeof(super));

	printf("Id: %.4s\n", super.id);
	printf("Version: %02X\n", super.version);
	printf("Super block size: %hu sectors\n", super.superblockSize);
	printf("Disk size: %u bytes\n", super.DiskSize);
	printf("Disk size: %u sectors\n", super.NofSectors);
	printf("Setors per cluster: %u sectors\n", super.SectorsPerCluster);
	printf("FAT start sector: %u\n", super.pFATSectorStart);
	printf("Root cluster: %u\n", super.RootDirCluster);
	printf("First data sector: %u\n\n", super.DataSectorStart);

	read_cluster(2, cluster);

	for(i = 0; i < 16; i++) {

		memcpy(&record, &cluster[i*64], sizeof(record));

		record.bytesFileSize = little_to_big(record.bytesFileSize);
		record.clustersFileSize = little_to_big(record.clustersFileSize);
		record.firstCluster = little_to_big(record.firstCluster);

		printf("\n\nType: %02X\n", record.TypeVal);
		printf("Name: %s\n", record.name);
		printf("File size: %hu bytes\n", record.bytesFileSize);
		printf("File size: %hu clusters\n", record.clustersFileSize);
		printf("First cluster: %hu\n", record.firstCluster);
	}

}

void read_cluster(int cluster, unsigned char *buffer) {
	int initial_sector = super.DataSectorStart + cluster * 4;
	int i, ret;

	printf("%d\n", initial_sector);

	for(i = 0; i < 4; i++) {
		ret = read_sector (initial_sector+i, &buffer[i*SECTOR_SIZE]);
		printf("%d\n", ret);
	}
}

// Converts 32 bits little endian to big endian
DWORD little_to_big(DWORD little) {
	DWORD big;

	big = ((little>>24)&0xff) | // move byte 3 to byte 0
          ((little<<8)&0xff0000) | // move byte 1 to byte 2
          ((little>>8)&0xff00) | // move byte 2 to byte 1
          ((little<<24)&0xff000000); // byte 0 to byte 3

    return big;
}