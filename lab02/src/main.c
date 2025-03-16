#include "collatz.h"
#include <stdio.h>

#define MAX_ITER 1000

void print_sequence(int *steps, int count) {
  for (int i = 0; i < count; i++) {
    printf("%d ", steps[i]);
  }
  printf("\n");
}

int main() {
  int steps[MAX_ITER];
  int input_values[] = {6, 17, 29, 50, 125, 6763155};
  int size = sizeof(input_values) / sizeof(input_values[0]);

  for (int i = 0; i < size; i++) {
    int num = input_values[i];
    printf("Testing %d:\n", num);
    int count = test_collatz_convergence(num, MAX_ITER, steps);
    if (count > 1) {
      print_sequence(steps, count);
    } else {
      printf("Could not reach 1 in %d steps\n", MAX_ITER);
    }
    printf("\n");
  }

  return 0;
}