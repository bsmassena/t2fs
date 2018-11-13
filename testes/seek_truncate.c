#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/t2fs.h"
#include "../include/apidisk.h"
#include "../include/filesystem.h"

int main() {
	FILE2 file;
	char end_of_string = '\0';
	char *content = "Primeira parte|Segunda parte", buffer[strlen(content) + 1];
	// Create the file
	file = create2("testFile");
	if(file < 0) {
		printf("ERRO CRIANDO O ARQUIVO.\n");
		exit(1);
	}

	// Write on the file
	if(write2(file, content, strlen(content) + 1) < 0) {
		printf("ERRO ESCREVENDO NO ARQUIVO.\n");
		exit(1);
	}

	// Seek to middle point
	if(seek2(file, 15) != 0) {
		printf("ERRO NO SEEK PARA O MEIO.\n");
		exit(1);
	}
	// Read after seek
	if(read2(file, buffer, 30) < 0) {
		printf("ERRO NA LEITURA DO ARQUIVO.\n");
	}
	// Comparation
	if(strcmp(buffer, &content[15]) == 0) {
		printf("Sucesso no primeiro seek. Lido: %s\n", buffer);
	} else {
		printf("ERRO, CONTEUDO ERRADO NO SEEK.\n");
		exit(0);
	}


	// Seek to middle point
	if(seek2(file, 15) != 0) {
		printf("ERRO NO SEEK PARA O MEIO.\n");
		exit(1);
	}
	// Add end of string sign
	if(write2(file, &end_of_string, 1) < 0) {
		printf("ERRO NA INSERCAO DO FIM DE STRING.\n");
		exit(1);
	}
	// Truncate
	if(truncate2(file) != 0) {
		printf("ERRO NO TRUNCATE.\n");
		exit(0);
	}
	// Seek to beggininf point
	if(seek2(file, 0) != 0) {
		printf("ERRO NO SEEK PARA O INICIO.\n");
		exit(1);
	}
	// Read after seek
	if(read2(file, buffer, 30) < 0) {
		printf("ERRO NA LEITURA DO ARQUIVO.\n");
	}
	// Comparation
	if(strcmp(buffer, "Primeira parte|") == 0) {
		printf("Sucesso na truncagem. Lido: %s\n", buffer);
	} else {
		printf("ERRO, CONTEUDO ERRADO DEPOIS DA TRUNCAGEM.\n");
		exit(0);
	}

	// Seek to the end
	if(seek2(file, -1) != 0) {
		printf("ERRO NO SEEK PARA O FIM.\n");
		exit(1);
	}
	if(write2(file, "Segunda parte", strlen("Segunda parte") + 1) < 0) {
		printf("ERRO NA INSERCAO DO FIM DE STRING.\n");
		exit(1);
	}
	if(seek2(file, 0) != 0) {
		printf("ERRO NO SEEK PARA O FIM.\n");
		exit(1);
	}
	// Read after seek
	if(read2(file, buffer, 30) < 0) {
		printf("ERRO NA LEITURA DO ARQUIVO.\n");
	}
	strcat(&buffer[strlen(buffer)], &buffer[strlen(buffer) + 1]);
	if(strcmp(buffer, content) == 0) {
		printf("Sucesso no seek para o final.\n");
	} else {
		printf("ERRO DEPOIS DO APPEND. ESPERADO: %s\tOBTIDO: %s\n", content, buffer);
	}


	return 0;
}
