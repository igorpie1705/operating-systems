#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

int global = 0;

int main(int argc, char *argv[]) {
  if (argc < 2) {
    fprintf(stderr, "Nie podano argumentu wejsciowego. \n");
    return EXIT_FAILURE;
  }

  struct stat sb;
  if (stat(argv[1], &sb) == -1 || !S_ISDIR(sb.st_mode)) {
    fprintf(stderr, "Podana sciezka nie jest prawidlowym katalogiem.\n");
    return EXIT_FAILURE;
  }

  printf("Nazwa pliku: %s\n", argv[0]);

  pid_t child_pid = fork();
  int local = 0;

  if (child_pid < 0) {
    perror("Błąd fork()");
    return EXIT_FAILURE;
  }
  if (child_pid == 0) {
    printf("child process\n");
    global++;
    local++;
    printf("child pid = %d, parent pid = %d\n", getpid(), getppid());
    printf("child's local = %d, child's global = %d\n", local, global);

    execl("/bin/ls", "ls", argv[1], NULL);
    perror("Blad execl()");
    return EXIT_FAILURE;

  } else {
    int status;
    waitpid(child_pid, &status, 0);

    printf("parent process\n");
    printf("parent pid = %d, child pid = %d\n", getpid(), child_pid);
    if (WIFEXITED(status)) {
      printf("Child exit code: %d\n", WEXITSTATUS(status));
    } else {
      printf("Child did not exit normally.\n");
    }
    printf("Parent's local = %d, parent's global = %d\n", local, global);

    return WEXITSTATUS(status);
  }
}