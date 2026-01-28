#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "user/user.h"

char buf[512];
int show_n = 0;      // Flag đánh dấu có dùng -n hay không
int line_number = 1; // Đếm số dòng

void print_padding_number(int num) {
  int temp = num;
  int digits = 0;
  
  if (temp == 0) digits = 1;
  while (temp > 0) {
    digits++;
    temp /= 10;
  }

  // In các khoảng trắng phía trước để tổng phần số chiếm 6 ký tự
  for (int i = 0; i < (6 - digits); i++) {
    printf(" ");
  }
  printf("%d  ", num); // Số dòng và 2 dấu cách sau đó
}

int readline(int fd, char *buf, int maxlen);

void cat(int fd) {
  int n;
  if (show_n) {
    while ((n = readline(fd, buf, sizeof(buf))) > 0) {
      print_padding_number(line_number++); // Gọi hàm căn lề
      printf("%s", buf);
    }
  } else {
    // Nếu không có -n, làm theo logic ban đầu: Gọi hết
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

  // Kiểm tra xem đối số đầu tiên có phải là -n không
  if (argc > 1 && strcmp(argv[1], "-n") == 0) {
    show_n = 1;
    arg_start = 2; // Bắt đầu đọc file từ đối số thứ 2
  }

  // Xử lý trường hợp đọc từ stdin (không có file)
  if (argc <= arg_start) {
    cat(0);
    exit(0);
  }

  // Xử lý trường hợp input nhiều file
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
