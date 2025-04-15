#include <fcntl.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

double f(double x) { return 4.0 / (x * x + 1); }

int main() {
  int to_fd = open("to_worker", O_RDONLY);
  if (to_fd == -1) {
    perror("Otworz to_worker");
    return EXIT_FAILURE;
  }

  double range[2];
  read(to_fd, range, sizeof(range));
  close(to_fd);

  double a = range[0];
  double b = range[1];
  double dx = 1e-8;
  double result = 0.0;

  for (double x = a; x < b; x += dx) {
    result += f(x) * dx;
  }

  int from_fd = open("from_worker", O_WRONLY);
  if (from_fd == -1) {
    perror("Otworz from_worker");
    return EXIT_FAILURE;
  }

  write(from_fd, &result, sizeof(result));
  close(from_fd);
  return EXIT_SUCCESS;
}