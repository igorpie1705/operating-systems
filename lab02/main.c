#include "collatz.h"
#include <dlfcn.h>
#include <stdio.h>

#define MAX_ITER 10

#ifdef DYNAMIC_LOAD
int (*ptr_collatz_conjecture)(int) = NULL;
int (*ptr_test_collatz_convergence)(int, int, int *) = NULL;
#endif

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

#ifdef DYNAMIC_LOAD
  void *handle = dlopen("./libcollatz.so", RTLD_LAZY);
  if (!handle) {
    fprintf(stderr, "Error when loading library: %s\n", dlerror());
    return 1;
  }
  ptr_collatz_conjecture = dlsym(handle, "collatz_conjecture");
  ptr_test_collatz_convergence = dlsym(handle, "test_collatz_convergence");

  if (dlerror() != NULL) {
    fprintf(stderr, "Error when loading function: %s\n", dlerror());
    dlclose(handle);
    return 1;
  }
#endif

  for (int i = 0; i < size; i++) {
    int num = input_values[i];
    printf("Testing %d:\n", num);
#ifdef DYNAMIC_LOAD
    int count = ptr_test_collatz_convergence(num, MAX_ITER, steps);
#else
    int count = test_collatz_convergence(num, MAX_ITER, steps);
#endif
    if (count > 0) {
      print_sequence(steps, count);
    } else {
      printf("Could not reach 1 in %d steps\n", MAX_ITER);
    }
    printf("\n");
  }

#ifdef DYNAMIC_LOAD
  dlclose(handle);
#endif

  return 0;
}