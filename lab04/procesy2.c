#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int global = 0;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Nie podano argumentu wejsciowego. \n");
    return -1;
  }

  // TODO Obsluga bledu nieprawidlowego katalogu w arg wejsciowym.

  printf("Nazwa pliku: \n");

  pid_t child_pid = fork();
  int local = 0;

  if (child_pid < 0) {
    perror("Błąd fork()");
    return 1;
  }
  if (child_pid == 0) {
    printf("child process\n");
    global++;
    local++;
    printf("child pid = %d, parent pid = %d\n", getpid(), getppid());
    printf("child's local = %d, child's global = %d\n", local, global);
    int status = execl(argv[1], "ls", NULL);
  } else {
    printf("parent process\n");
    printf("parent pid = %d, child pid = %d\n", getpid(), child_pid);
    printf("Child exit code: %d\n"); // TODO dodac status
    printf("Parent's local = %d, parent's global = %d\n", local, global);
    // TODO zwroc stosowny kod bledu
  }

  wait(NULL);
  return 0;
}