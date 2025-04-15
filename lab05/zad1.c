#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef enum { NONE, IGNORE, HANDLER, MASK } options;

void handler() { printf("Otrzymano sygnal SIGUSR1. \n"); }

options get_mode(const char *arg) {
  if (strcmp(arg, "ignore") == 0)
    return IGNORE;
  if (strcmp(arg, "handler") == 0)
    return HANDLER;
  if (strcmp(arg, "mask") == 0)
    return MASK;
  if (strcmp(arg, "none") == 0)
    return NONE;
  printf("Podano nieprawidlowy argument. \n");
  exit(EXIT_FAILURE);
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Nie podano wystarczajacej liczby argumentow. \n");
    exit(EXIT_FAILURE);
  }

  options mode = get_mode(argv[1]);
  sigset_t sigset;
  struct sigaction act;

  switch (mode) {
  case IGNORE:
    signal(SIGUSR1, SIG_IGN);
    break;
  case HANDLER: {
    act.sa_handler = handler;
    sigemptyset(&act.sa_mask);
    act.sa_flags = 0;
    sigaction(SIGUSR1, &act, NULL);
    break;
  }
  case MASK:
    sigemptyset(&sigset);
    sigaddset(&sigset, SIGUSR1);
    sigprocmask(SIG_BLOCK, &sigset, NULL);
    break;
  case NONE:
    break;
  default:
    fprintf(stderr, "Nieprawidlowy argument.\n");
    exit(EXIT_FAILURE);
  }

  printf("Wykonywanie raise...\n");
  raise(SIGUSR1);

  if (mode == MASK) {
    sigpending(&sigset);
    if (sigismember(&sigset, SIGUSR1)) {
      printf("SIGUSR1 jest widoczny i w trybie oczekiwania.\n");
    }
  }
  return 0;
}