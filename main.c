#include <stdio.h>
#include <stdlib.h>

int main() {
  char fileName[100];
  FILE: *fp;
  
  printf("Enter filename: ");
  scanf("%s", fileName);

  fp = fopen(fileName,"r");
  
  if (fp == NULL) {
    printf ("%s not found.\n", fileName);
    exit(0);
  } else {
    
  }
  return 0;
}