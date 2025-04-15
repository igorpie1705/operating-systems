#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

volatile sig_atomic_t received_ack = 0;

void handle_ack(int signum) {
  (void)signum;
  received_ack = 1;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Użycie: %s <PID catchera> <tryb>\n", argv[0]);
    return EXIT_FAILURE;
  }

  pid_t catcher_pid = atoi(argv[1]);
  int mode = atoi(argv[2]);

  if (mode < 1 || mode > 5) {
    printf("Nieprawidłowy tryb (1-5)\n");
    return EXIT_FAILURE;
  }

  struct sigaction sa;
  sa.sa_handler = handle_ack;
  sigemptyset(&sa.sa_mask);
  sa.sa_flags = 0;
  sigaction(SIGUSR1, &sa, NULL);

  union sigval value;
  value.sival_int = mode;
  if (sigqueue(catcher_pid, SIGUSR1, value) == -1) {
    perror("sigqueue");
    return EXIT_FAILURE;
  }

  sigset_t mask, oldmask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR1);
  sigprocmask(SIG_BLOCK, &mask, &oldmask);

  while (!received_ack) {
    sigsuspend(&oldmask);
  }

  printf("Potwierdzenie odebrane. Sender konczy dzialanie.\n");
  return EXIT_SUCCESS;
}
