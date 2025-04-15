#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

volatile sig_atomic_t mode_change_count = 0;
volatile sig_atomic_t current_mode = 0;
volatile sig_atomic_t running = 1;
volatile sig_atomic_t got_signal = 0;

pid_t sender_pid = -1;

void ctrlc_ignore() {}

void ctrlc_message() { printf("\nWcisnieto CTRL + C\n"); }

void sigusr1_handler(int signum, siginfo_t *info, void *ucontext) {
  (void)signum;
  (void)ucontext;

  sender_pid = info->si_pid;
  int received_mode = info->si_value.sival_int;
  current_mode = received_mode;
  mode_change_count++;
  got_signal = 1;
}

void set_ctrlc_handler(int mode) {
  struct sigaction sa;
  sa.sa_flags = 0;
  sigemptyset(&sa.sa_mask);

  if (mode == 3) {
    sa.sa_handler = ctrlc_ignore;
  } else if (mode == 4) {
    sa.sa_handler = ctrlc_message;
  } else {
    sa.sa_handler = SIG_DFL;
  }

  sigaction(SIGINT, &sa, NULL);
}

int main() {
  printf("PID catchera: %d\n", getpid());

  struct sigaction sa;
  sa.sa_sigaction = sigusr1_handler;
  sa.sa_flags = SA_SIGINFO;
  sigemptyset(&sa.sa_mask);
  sigaction(SIGUSR1, &sa, NULL);

  sigset_t mask, oldmask;
  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR1);
  sigprocmask(SIG_BLOCK, &mask, &oldmask);

  while (running) {
    got_signal = 0;
    sigsuspend(&oldmask);

    if (got_signal && sender_pid != -1) {
      switch (current_mode) {
      case 1:
        printf("Liczba otrzymanych żądań zmiany trybu: %d\n",
               mode_change_count);
        break;
      case 2:
        for (int i = 1; current_mode == 2; ++i) {
          printf("%d\n", i);
          sleep(1);
          if (!running)
            break;
        }
        break;
      case 3:
        set_ctrlc_handler(3);
        break;
      case 4:
        set_ctrlc_handler(4);
        break;
      case 5:
        running = 0;
        break;
      default:
        printf("Nieznany tryb: %d\n", current_mode);
        break;
      }
      union sigval value;
      value.sival_int = 0;
      sigqueue(sender_pid, SIGUSR1, value);
    }
  }

  printf("Catcher konczy dzialanie.\n");
  return 0;
}
