#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define A 0.0
#define B 1.0

double f(double x) { return 4 / (x * x + 1); }

void compute_integral(double dx, int k) {

  struct timeval start, end;
  gettimeofday(&start, NULL);

  int pipes[k][2];
  double total = 0.0;

  for (int i = 0; i < k; i++) {
    if (pipe(pipes[i]) == -1) {
      perror("pipe");
      exit(EXIT_FAILURE);
    }

    pid_t pid = fork();
    if (pid == -1) {
      perror("fork");
      exit(EXIT_FAILURE);
    }

    if (pid == 0) {
      close(pipes[i][0]);

      double partial_sum = 0.0;
      double a = A + i * (B - A) / k;
      double b = A + (i + 1) * (B - A) / k;

      for (double x = a; x < b; x += dx) {
        partial_sum += f(x + dx / 2) * dx;
      }

      write(pipes[i][1], &partial_sum, sizeof(partial_sum));
      close(pipes[i][1]);
      exit(EXIT_SUCCESS);
    } else {
      close(pipes[i][1]);
    }
  }

  for (int i = 0; i < k; i++) {
    double partial_sum;
    read(pipes[i][0], &partial_sum, sizeof(partial_sum));
    total += partial_sum;
    close(pipes[i][0]);
  }
  for (int i = 0; i < k; i++) {
    wait(NULL);
  }

  gettimeofday(&end, NULL);

  double time_spent =
      (end.tv_sec - start.tv_sec) + (end.tv_usec - start.tv_usec) / 1e6;

  printf("k=%d\t wynik=%.10f\t czas=%.4f s\n", k, total, time_spent);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Użycie: %s szerokość_prostokąta liczba_procesów\n",
            argv[0]);
    exit(EXIT_FAILURE);
  }

  double dx = atof(argv[1]);
  int k_max = atoi(argv[2]);

  if (dx <= 0 || k_max <= 0) {
    fprintf(stderr,
            "Szerokość prostokąta i liczba procesów muszą być dodatnie\n");
    exit(EXIT_FAILURE);
  }

  for (int k = 1; k <= k_max; k++) {
    compute_integral(dx, k);
  }

  return 0;
}