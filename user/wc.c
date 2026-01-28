#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];

// option flags (global so wc() can see them)
int show_l = 0, show_w = 0, show_c = 0;

void wc(int fd, char *name) {
  int i, n;
  int l = 0, w = 0, c = 0;
  int inword = 0;

  while ((n = read(fd, buf, sizeof(buf))) > 0) {
    for (i = 0; i < n; i++) {
      c++;
      if (buf[i] == '\n')
        l++;
      if (strchr(" \r\t\n\v", buf[i]))
        inword = 0;
      else if (!inword) {
        w++;
        inword = 1;
      }
    }
  }

  if (n < 0) {
    printf("wc: read error\n");
    exit(1);
  }

  if (show_l)
    printf("%d ", l);
  if (show_w)
    printf("%d ", w);
  if (show_c)
    printf("%d ", c);

  if (name[0] != 0)
    printf("%s", name);

  printf("\n");
}

int main(int argc, char *argv[]) {
  int fd;
  int i = 1;

  // parse options
  while (i < argc && argv[i][0] == '-') {
    if (strcmp(argv[i], "-l") == 0)
      show_l = 1;
    else if (strcmp(argv[i], "-w") == 0)
      show_w = 1;
    else if (strcmp(argv[i], "-c") == 0)
      show_c = 1;
    else {
      printf("wc: unknown option %s\n", argv[i]);
      continue;
    }
    i++;
  }

  // default: show all
  if (!show_l && !show_w && !show_c) {
    show_l = show_w = show_c = 1;
  }

  // stdin case
  if (i == argc) {
    wc(0, "");
    exit(0);
  }

  // files
  for (; i < argc; i++) {
    if ((fd = open(argv[i], O_RDONLY)) < 0) {
      printf("wc: cannot open %s\n", argv[i]);
      continue;
    }
    wc(fd, argv[i]);
    close(fd);
  }

  exit(0);
}
