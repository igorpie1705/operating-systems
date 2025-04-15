#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

void reverse_line(char *line) {
  size_t len = strlen(line);
  if (len > 0 && line[len - 1] == '\n') {
    line[len - 1] = '\0'; // Usuniecie znaku nowej linii
    len--;
  }

  for (size_t i = 0; i < len / 2; i++) {
    char temp = line[i];
    line[i] = line[len - i - 1];
    line[len - i - 1] = temp;
  }
}

void process_file(const char *input_path, const char *output_path) {
  FILE *input_file = fopen(input_path, "r");
  if (!input_file) {
    perror("Error opening input file");
    return;
  }

  FILE *output_file = fopen(output_path, "w");
  if (!output_file) {
    perror("Error opening output file");
    fclose(input_file);
    return;
  }

  char buffer[1024];
  while (fgets(buffer, sizeof(buffer), input_file)) {
    reverse_line(buffer);
    fprintf(output_file, "%s\n", buffer);
  }

  fclose(input_file);
  fclose(output_file);
}

void process_directory(const char *source_dir, const char *output_dir) {
  DIR *dir = opendir(source_dir);
  if (!dir) {
    perror("Error opening source directory");
    return;
  }

  struct dirent *entry;
  struct stat statbuf;
  char input_path[1024], output_path[1024];

  mkdir(output_dir, 0755);

  while ((entry = readdir(dir)) != NULL) {
    snprintf(input_path, sizeof(input_path), "%s/%s", source_dir,
             entry->d_name);
    snprintf(output_path, sizeof(output_path), "%s/%s", output_dir,
             entry->d_name);

    if (stat(input_path, &statbuf) == 0) {
      if (strstr(entry->d_name, ".txt")) {
        process_file(input_path, output_path);
      }
    }
  }
  closedir(dir);
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Nieprawidlowa liczba argumentow \n");
    return EXIT_FAILURE;
  }

  process_directory(argv[1], argv[2]);
  printf("Zakonczono procesowanie katalogu \n");

  return EXIT_SUCCESS;
}