#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./include/t2fs.h"
#include "./include/apidisk.h"
#include "./include/filesystem.h"

void wrong(char filename[]) {
  if(file_name_is_valid(filename)) {
    printf("%s deveria ser invalido\n", filename);
  } else {
    printf("Ok pra %s\n", filename);
  }
}

void right(char filename[]) {
  if(file_name_is_valid(filename)) {
    printf("Ok pra %s\n", filename);
  } else {
    printf("%s deveria ser valido.\n", filename);
  }
}

void wrong_path(char path[]) {
  if(path_is_valid(path)) {
    printf("%s deveria ser invalido\n", path);
  } else {
    printf("OK pra %s\n", path);
  }
}

void right_path(char path[]) {
  if(path_is_valid(path)) {
    printf("Ok pra %s\n", path);
  } else {
    printf("%s deveria ser valido.\n", path);
  }
}

int main() {
  char name1[] = "abcdefghijklmnopqrstuvwxyz";
  char name2[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
  char name3[] = "0123456789";
  char name4[] = "mixedTypes123";
  char name5[] = "with_invalid_character";
  char name6[] = "toLong0000000000000000000000000000000000000000000000000";
  char name7[] = "limitLength0000000000000000000000000000000000000000000";

  char path1[] = "file/to/path";
  char path2[] = "ends/with/divisor/";
  char path3[] = "/absolute/path";
  char path4[] = "/invalid/absolut_path";

  printf("FILENAMES\n");
  right(name1);
  right(name2);
  right(name3);
  right(name4);
  wrong(name5);
  wrong(name6);
  right(name7);

  printf("\nPATH NAMES\n");
  right_path(path1);
  wrong_path(path2);
  right_path(path3);
  wrong_path(path4);
  return 0;
}
