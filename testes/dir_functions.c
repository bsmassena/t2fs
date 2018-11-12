#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/t2fs.h"
#include "../include/apidisk.h"
#include "../include/filesystem.h"

int main() {
  DIR2 dir;
  DIRENT2 entrada;
  if(mkdir2("testDir") != 0) {
    printf("ERRO NA CRIACAO DO DIRETORIO DE TESTE.\n");
    exit(1);
  }
  dir = opendir2("/");
  if(dir < 0) {
    printf("ERRO NA ABERTURA DO DIRETORIO DE TESTE.\n");
    exit(1);
  }
  if(readdir2(dir, &entrada) != 0) {
    printf("ERRO NA LEITURA DO DIRETORIO DE TESTE.\n");
    exit(1);
  }
  if(strcmp(entrada.name, ".") == 0) {
    printf("Leitura correta do diretorio.\n");
  } else {
    printf("ERRO NA LEITURA DO DIRETORIO.\n");
  }
  if(closedir2(dir) < 0) {
    printf("ERRO AO FECHAR O DIRETORIO.\n");
  }
  return 0;
}
