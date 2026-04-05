#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(int argc, char *argv[])
{
  uint64 a0, a1;
  int *p0, *p1;
  int pid;

  (void)argc;
  (void)argv;

  printf("Testing multiple mmap regions...\n");

  a0 = mmap();
  a1 = mmap();
  if (a0 == 0 || a1 == 0 || a0 == a1) {
    printf("mmap allocation failed\n");
    exit(1);
  }
  printf("Mapped region 0 at 0x%lx\n", a0);
  printf("Mapped region 1 at 0x%lx\n", a1);

  p0 = (int*)a0;
  p1 = (int*)a1;
  *p0 = 11;
  *p1 = 22;

  pid = fork();
  if (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    printf("Child saw: %d %d\n", *p0, *p1);
    *p0 = 33;
    *p1 = 44;
    munmap(a0);
    munmap(a1);
    exit(0);
  }

  wait(0);
  printf("Parent saw: %d %d\n", *p0, *p1);
  munmap(a0);
  munmap(a1);
  exit(0);
}
