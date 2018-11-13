#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/t2fs.h"
#include "../include/apidisk.h"
#include "../include/filesystem.h"

int main() {
  FILE2 arq;
  char conteudo[] = "Conteudo do arquivo";
  char resultado[strlen(conteudo) + 1];

  mkdir2("diretorio");
  mkdir2("outroDiretorio");
  chdir2("diretorio");
  mkdir2("diretorio2");
  chdir2("diretorio2");

  arq = create2("arquivo");
  if(write2(arq, conteudo, strlen(conteudo) + 1) != strlen(conteudo) + 1) {
    printf("Erro na escrita do arquivo.\n");
    exit(1);
  }

  close2(arq);
  chdir2("/");
  if(ln2("link", "diretorio/diretorio2/arquivo") != 0) {
    printf("Erro na criacao do link.\n");
    exit(1);
  }
  if((arq = open2("link")) < 0) {
    printf("Erro na abertura do link\n");
    exit(1);
  }
  if(read2(arq, resultado, strlen(conteudo) + 1) < 0) {
    printf("Erro na leitura do link.\n");
    exit(1);
  }
  printf("O conteudo do link eh: %s| %d bytes\n", resultado, strlen(resultado));
  return 0;
}
