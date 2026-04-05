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
  printf("Distinct shared regions: %s\n", a0 != a1 ? "yes" : "no");

  p0 = (int*)a0;
  p1 = (int*)a1;
  *p0 = 11;
  *p1 = 22;
  printf("Parent wrote region 0: %d\n", *p0);
  printf("Parent wrote region 1: %d\n", *p1);

  pid = fork();
  if (pid < 0) {
    printf("fork failed\n");
    exit(1);
  }

  if (pid == 0) {
    printf("Child read region 0: %d\n", *p0);
    printf("Child read region 1: %d\n", *p1);
    *p0 = 33;
    *p1 = 44;
    printf("Child wrote region 0: %d\n", *p0);
    printf("Child wrote region 1: %d\n", *p1);

    if (munmap(a0) < 0)
      printf("Child: munmap region 0 failed\n");
    else
      printf("Child: munmap region 0 succeeded\n");

    if (munmap(a1) < 0)
      printf("Child: munmap region 1 failed\n");
    else
      printf("Child: munmap region 1 succeeded\n");

    exit(0);
  }

  wait(0);
  printf("Parent read region 0 after child: %d\n", *p0);
  printf("Parent read region 1 after child: %d\n", *p1);

  if (munmap(a0) < 0)
    printf("Parent: munmap region 0 failed\n");
  else
    printf("Parent: munmap region 0 succeeded\n");

  if (munmap(a1) < 0)
    printf("Parent: munmap region 1 failed\n");
  else
    printf("Parent: munmap region 1 succeeded\n");

  exit(0);
}
