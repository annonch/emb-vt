#include <stdio.h>
#include <stdlib.h>
#include <string.h>
/*
int main(int argc, char** argv) {
  int pids[128];
  int i = 0;
  for(i;i<128;i++) {
    pids[i] = i;
  }



}

char** str_split(char* a_str,

*/

/*
int main ()
{
    char buf[] ="abc/qwe/ccd";
    int i = 0;
    char *p = strtok (buf, "/");
    char *array[4];

    while (p != NULL)
    {
        array[i++] = p;
        p = strtok (NULL, "/");
    }

    for (i = 0; i < 4; ++i)
        printf("%s\n", array[i]);

    return 0;
}
//*/


int main() {
  return str2int();
}
int str2int(){
  int pids[128];
  int p=0;
  int j=0;
  int count = 0;
  const char *str = "345 3345 35 75 52 386 50 40 30 20 10 2 399";
  char *dst = (char*) malloc(43*sizeof(char));

  strncpy(dst,str,43);
  char *end = dst;
  while(*end) {
    int n = strtol(str, &end, 10);
    pids[count]=n;
    count++;
    printf("%d\n", n);
    while (*end == ' ') {
      end++;
    }
    str = end;
  }
  printf("\n");
  for(j=0;j < count;j++){
    printf("%d\n", pids[j]);

  }
  free(dst);
  return 0;
}
