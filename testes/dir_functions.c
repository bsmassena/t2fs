#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/t2fs.h"
#include "../include/apidisk.h"
#include "../include/filesystem.h"

DIR2 test_open(char *pathname) {
  DIR2 dir;
  dir = opendir2(pathname);
  if(dir < 0) {
    printf("ERRO NA ABERTURA DO DIRETORIO COM PATH %s\n", pathname);
  }
  return dir;
}

void test_read(DIR2 dir, DIRENT2 *entry) {
  if(readdir2(dir, entry) != 0) {
    printf("ERRO NA LEITURA DO DIRETORIO DE TESTE.\n");
    exit(1);
  }
}

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
  dir = test_open("./directory");

  // For each element inside new dir, compare name
  for(i = 0; i <= 3; i++) {
    test_read(dir, &entrada);
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
  other_dir = test_open("/directory");
  closedir2(other_dir);
  // 2. Without ./
  other_dir = opendir2("directory");
  closedir2(other_dir);
  return 0;
}
