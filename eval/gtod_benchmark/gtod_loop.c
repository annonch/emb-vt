#include <sys/time.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>


// stuff for ftrace
int trace_fd = -1;
int marker_fd = -1;

#define NUM_RUNS 1000000
#define MAX_PATH 256
#define _STR(x) #x
#define STR(x) _STR(x)


static char *find_debugfs(void)
{
  static char debugfs[MAX_PATH+1];
  static int debugfs_found;
  char type[100];
  FILE *fp;

  if (debugfs_found)
    return debugfs;

  if ((fp = fopen("/proc/mounts","r")) == NULL)
    return NULL;

  while (fscanf(fp, "%*s %"
		STR(MAX_PATH)
		"s %99s %*s %*d %*d\n",
		debugfs, type) == 2) {
    if (strcmp(type, "debugfs") == 0)
      break;
  }
  fclose(fp);

  if (strcmp(type, "debugfs") != 0)
    return NULL;

  debugfs_found = 1;

  return debugfs;
}


long double gt(){

  struct timeval tv[NUM_RUNS];

  time_t curtime, starttime;
  suseconds_t ms, startms;
  long double res;
  volatile int x,y;
  long int misc_total;
  
  
  write(trace_fd, "1", 1);
  write(marker_fd, "Before GTOD\n", 12);

  gettimeofday(&tv[0], NULL);
  starttime=tv[0].tv_sec;
  startms=tv[0].tv_usec;

  //nanosleep(&req, NULL);
  for(y=0;y<10;y++){
    for(x=0;x<NUM_RUNS;x++) {
      gettimeofday(&tv[x], NULL);
    }
  }
  
  write(marker_fd, "After GTOD\n", 11);
  write(trace_fd, "0", 1);

  curtime=tv[NUM_RUNS-1].tv_sec;
  ms=tv[NUM_RUNS-1].tv_usec;

  if(ms > startms) {
    printf("%ld\n",((long)curtime - (long)starttime));
    printf("%ld\n",((long)ms - (long)startms));
  }
  else {
    printf("%ld\n",((long)curtime - 1 - (long)starttime));
    printf("%ld\n",((long)ms+1000000 - (long)startms));
  }

  for(x=2;x<NUM_RUNS;x++) {
    misc_total += tv[x].tv_sec - tv[x-1].tv_usec ;
  }
  printf("ignore this number (trying to trick the compiler) %ld\n",(long)misc_total);
  
  /*
  printf("startTime:\n");
  printf("%ld\n",(long)starttime);
  printf("%ld\n",(long)startms);

  printf("endTime:\n");
  printf("%ld\n",(long)curtime);
  printf("%ld\n",(long)ms);
  */
  res = ((long double) curtime + (long double)ms/1000000.0);
  //printf("%Lf\n", res);
  return res;
}

int  main(){
  //ftrace stuff
  char *debugfs;
  char path[256];
  
  debugfs = find_debugfs();
  if (debugfs) {
    strcpy(path, debugfs);  /* BEWARE buffer overflow */
    strcat(path,"/tracing/tracing_on");
    trace_fd = open(path, O_WRONLY);
    //if (trace_fd >= 0)
    //  write(trace_fd, "1", 1);
    
    strcpy(path, debugfs);
    strcat(path,"/tracing/trace_marker");
    marker_fd = open(path, O_WRONLY);
  }

  gt();
  return 0;
}
