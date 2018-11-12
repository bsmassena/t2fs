#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/t2fs.h"
#include "../include/apidisk.h"
#include "../include/filesystem.h"

int main() {
  DIR2 dir, other_dir;
  DIRENT2 entrada;
  int i;
  char names[][9] = { ".", "..", "testDir", "testFile" };

  // Create dir to controll environment
  if(mkdir2("directory") != 0) {
    printf("ERRO NA CRIACAO DO DIRETORIO DE TESTE.\n");
    exit(1);
  }
  // Create dir inside new dir
  if(mkdir2("directory/testDir") != 0) {
    printf("ERRO NA CRIACAO DO DIRETORIO DE TESTE.\n");
    exit(1);
  }
  // Create file inside new dir
  if(create2("directory/testFile") < 0) {
    printf("ERRO NA CRIACAO DO ARQUIVO DE TESTE.\n");
    exit(1);
  }
  // Open new dir
  dir = opendir2("./directory");
  if(dir < 0) {
    printf("ERRO NA ABERTURA DO DIRETORIO DE TESTE.\n");
    exit(1);
  }

  // For each element inside new dir, compare name
  for(i = 0; i <= 3; i++) {
    if(readdir2(dir, &entrada) != 0) {
      printf("ERRO NA LEITURA DO DIRETORIO DE TESTE.\n");
      exit(1);
    }
    if(strcmp(entrada.name, names[i]) == 0) {
      printf("Leitura correta da entrada %s.\n", entrada.name);
    } else {
      printf("ERRO NA LEITURA DO DIRETORIO.\n");
    }
  }

  if(closedir2(dir) < 0) {
    printf("ERRO AO FECHAR O DIRETORIO.\n");
  }

  // Test other ways to open dir
  // 1. Absolute
  other_dir = opendir2("/directory");
  if(other_dir != dir) {
    printf("ERRO NA ABERTURA DE DIRETORIO COM CAMINHO ABSOLUTO.\n");
  }
  // 2. Without ./
  other_dir = opendir2("directory");
  if(other_dir != dir) {
    printf("ERRO NA ABERTURA DE DIRETORIO COM CAMINHO RELATIVO SEM ./\n");
  }
  closedir2(other_dir);
  return 0;
}
