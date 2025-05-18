#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

double dx;
int max_threads;

double f(double x) { return 4.0 / (x * x + 1.0); }

typedef struct {
  int thread_id;
  double a;
  double b;
  double partial_sum;
} ThreadData;

void *compute_integral(void *arg) {
  ThreadData *data = (ThreadData *)arg;
  double a = data->a;
  double b = data->b;
  double sum = 0.0;

  for (double x = a; x < b; x += dx) {
    sum += f(x) * dx;
  }

  data->partial_sum = sum;
  pthread_exit(NULL);
}

double calculate_integral(int num_threads) {
  pthread_t threads[num_threads];
  ThreadData thread_data[num_threads];
  double total = 0.0;

  double interval = 1.0 / num_threads;

  for (int i = 0; i < num_threads; i++) {
    thread_data[i].thread_id = i;
    thread_data[i].a = i * interval;
    thread_data[i].b = (i + 1) * interval;

    pthread_create(&threads[i], NULL, compute_integral, &thread_data[i]);
  }

  for (int i = 0; i < num_threads; i++) {
    pthread_join(threads[i], NULL);
    total += thread_data[i].partial_sum;
  }

  return total;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr,
            "Użycie: %s szerokość_prostokąta maksymalna_liczba_wątków\n",
            argv[0]);
    return 1;
  }

  dx = atof(argv[1]);
  max_threads = atoi(argv[2]);

  if (dx <= 0 || max_threads <= 0) {
    fprintf(stderr, "Parametry muszą być dodatnie.\n");
    return 1;
  }

  for (int k = 1; k <= max_threads; k++) {
    clock_t start = clock();
    double result = calculate_integral(k);
    clock_t end = clock();

    double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
    printf("k=%d\t wynik=%.10f\t czas=%.4f s\n", k, result, time_spent);
  }

  return 0;
}