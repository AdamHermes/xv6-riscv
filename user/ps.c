// user/ps.c
#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"
#include "kernel/getproc.h"

// Convert process state enum to string
const char* state_to_string(enum procstate state) {
  switch(state) {
    case UNUSED:   return "unused";
    case USED:     return "used";
    case SLEEPING: return "sleep";
    case RUNNABLE: return "runble";
    case RUNNING:  return "run";
    case ZOMBIE:   return "zombie";
    default:       return "???";
  }
}

int main(int argc, char *argv[]) {
  struct procinfo procs[64];
  int num_procs;
  
  // Call the getprocs system call
  num_procs = getprocs(procs, 64);
  
  if (num_procs < 0) {
    fprintf(2, "ps: getprocs failed\n");
    exit(1);
  }
  
  // Print header
  printf("PID\tSTATE\tSIZE\tNAME\n");
  
  // Print process information
  for (int i = 0; i < num_procs; i++) {
    printf("%d\t%s\t%ld\t%s\n",
           procs[i].pid,
           state_to_string(procs[i].state),
           procs[i].sz,
           procs[i].name);
  }
  
  exit(0);
}
