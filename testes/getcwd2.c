#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/t2fs.h"
#include "../include/apidisk.h"
#include "../include/filesystem.h"

void muda(char *path) {
  char buffer[256] = "\0";
  printf("Muda pra %s\n", path);
  chdir2(path);
  getcwd2(buffer, 255);
  printf("%s\n\n", buffer);
}

int main() {
  mkdir2("diretorio");
  chdir2("diretorio");
  mkdir2("subdiretorio");
  chdir2("subdiretorio");
  mkdir2("terceiroNivel");
  chdir2("../");
  mkdir2("outroSubdiretorio");

  printf("Estrutura formada:\ndiretorio:\n\tsubdiretorio\n\t\tterceiroNivel\n\toutroSubdiretorio\n\n");

  muda("/");
  muda("./diretorio");
  muda("outroSubdiretorio");
  muda("../subdiretorio/terceiroNivel");
  muda("/diretorio/outroSubdiretorio");

  return 0;
}
