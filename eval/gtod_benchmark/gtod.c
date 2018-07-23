#include <sys/time.h>
#include <stdio.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>

// stuff for ftrace
int trace_fd = -1;
int marker_fd = -1;

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

  struct timeval tv;

  time_t curtime;
  suseconds_t ms;
  long double res;

  
  write(marker_fd, "before GTOD\n", 12);
  //nanosleep(&req, NULL);
  gettimeofday(&tv, NULL);
  write(marker_fd, "after GTOD\n", 11);
  write(trace_fd, "0", 1);
  
  
  //curtime=tv.tv_sec;
  //ms=tv.tv_usec;

  //printf("%ld\n",(long)curtime);
  //printf("%ld\n",(long)ms);
  //res = ((long double) curtime + (long double)ms/1000000.0);
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
    if (trace_fd >= 0)
      write(trace_fd, "1", 1);
    
    strcpy(path, debugfs);
    strcat(path,"/tracing/trace_marker");
    marker_fd = open(path, O_WRONLY);
  }

  gt();
  return 0;
}
