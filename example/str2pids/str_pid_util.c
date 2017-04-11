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
  char *str = "345 3345 35 75 52 386";
  char *end = str;
  while(*end) {
    int n = strtol(str, &end, 10);
    pids[p]=n;
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
  return 0;
}
