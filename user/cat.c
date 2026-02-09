#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];
int show_n = 0;      // -n flag
int line_number = 1; // Line counting

void formating(int num) {
  int temp = num;
  int digits = 0;
  
  if (temp == 0) digits = 1;
  while (temp > 0) {
    digits++;
    temp /= 10;
  }

  // Padding to follow the required format
  for (int i = 0; i < (6 - digits); i++) {
    printf(" ");
  }
  printf("%d  ", num); // Line number and padding
}

int readline(int fd, char *buf, int maxlen);

void cat(int fd) {
  int n;
  if (show_n) {
    while ((n = readline(fd, buf, sizeof(buf))) > 0) {
      formating(line_number++);
      printf("%s", buf);
    }
  } else {
    // If no -n, we copy the file directly
    while ((n = read(fd, buf, sizeof(buf))) > 0) {
      if (write(1, buf, n) != n) {
        fprintf(2, "cat: write error\n");
        exit(1);
      }
    }
  }
}

int readline(int fd, char *buf, int maxlen) {
  int n;
  char c;
  int i = 0;

  while ((n = read(fd, &c, 1)) > 0) {
    buf[i] = c;
    if (c == '\n') {
      break;
    }
    i += 1;
    if (i >= (maxlen - 1)) {
      fprintf(2, "readline() - line too long\n");
      exit(-1);
    }
  }

  if (((n == 0) && (i == 0)) || (n < 0))
    return n;

  i += 1;
  buf[i] = '\0';
  return i;
}

int main(int argc, char *argv[]) {
  int fd, i;
  int arg_start = 1;

  // Check the first argument for -n flag
  if (argc > 1 && strcmp(argv[1], "-n") == 0) {
    show_n = 1;
    arg_start = 2; // Read file from second argument
  }

  // Stdin reading (case: from file)
  if (argc <= arg_start) {
    cat(0);
    exit(0);
  }

  // Multi-file reading
  for (i = arg_start; i < argc; i++) {
    if ((fd = open(argv[i], O_RDONLY)) < 0) {
      fprintf(2, "cat: cannot open %s\n", argv[i]);
      exit(1);
    }
    cat(fd);
    close(fd);
  }
  exit(0);
}
