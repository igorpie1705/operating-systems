#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main() {
  double a, b;
  printf("Podaj przedzial calkowania [a b]: ");
  if (scanf("%lf %lf", &a, &b) != 2) {
    fprintf(stderr, "Błąd wczytywania danych.\n");
    return EXIT_FAILURE;
  }

  int to_fd = open("to_worker", O_WRONLY);
  if (to_fd == -1) {
    perror("Otworz plik to_worker");
    return EXIT_FAILURE;
  }

  double range[2] = {a, b};
  write(to_fd, range, sizeof(range));
  close(to_fd);

  int from_fd = open("from_worker", O_RDONLY);
  if (from_fd == -1) {
    perror("Otworz from_worker");
    return EXIT_FAILURE;
  }

  double result;
  read(from_fd, &result, sizeof(result));
  close(from_fd);

  printf("Wynik calkowania: %.10f\n", result);
  return EXIT_SUCCESS;
}