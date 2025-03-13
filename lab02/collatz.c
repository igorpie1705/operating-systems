int collatz_conjecture(int input) {
  if (input % 2 == 0) {
    input = input / 2;
  } else {
    input = input * 3 + 1;
  }
  return input;
}

int test_collatz_convergence(int input, int max_iter, int *steps) {
  for (int i; i < max_iter; i++) {
    input = collatz_conjecture(input);
  }
  return 0; // jeszcze nie jest gotowe
}
