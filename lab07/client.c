#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#define MAX_MSG_SIZE 256
#define SERVER_KEY 12345

// Typy komunikatów
#define INIT 1
#define MESSAGE 2

// Struktura komunikatu od klienta do serwera
typedef struct {
  long mtype;              // Typ komunikatu (INIT lub MESSAGE)
  int client_id;           // ID klienta (ignorowane dla INIT)
  key_t client_queue;      // Klucz kolejki klienta (tylko dla INIT)
  char text[MAX_MSG_SIZE]; // Treść komunikatu
} ClientMessage;

// Struktura komunikatu od serwera do klienta
typedef struct {
  long mtype;              // Typ komunikatu (client_id lub 1)
  int client_id;           // ID klienta nadawcy
  char text[MAX_MSG_SIZE]; // Treść komunikatu
} ServerMessage;

int client_queue_id;
int server_queue_id;
int client_id = -1;
pid_t child_pid;

// Obsługa zakończenia programu
void cleanup() {
  // Usunięcie kolejki klienta
  if (client_queue_id != -1) {
    if (msgctl(client_queue_id, IPC_RMID, NULL) == -1) {
      perror("Błąd podczas usuwania kolejki klienta");
    }
  }

  // Zabicie procesu potomnego, jeśli istnieje
  if (child_pid > 0) {
    kill(child_pid, SIGTERM);
    waitpid(child_pid, NULL, 0);
  }

  printf("Klient zakończył działanie.\n");
  exit(0);
}

// Obsługa sygnałów
void handle_signal(int sig) {
  printf("\nOtrzymano sygnał %d. Kończenie pracy klienta...\n", sig);
  cleanup();
}

// Funkcja odbierająca komunikaty od serwera
void receive_messages() {
  ServerMessage msg;

  while (1) {
    // Odbieranie komunikatu od serwera
    if (msgrcv(client_queue_id, &msg, sizeof(ServerMessage) - sizeof(long), 1,
               0) == -1) {
      perror("Błąd podczas odbierania komunikatu");
      exit(1);
    }

    if (msg.client_id == client_id) {
      // Wiadomość od serwera do tego klienta
      printf("%s\n", msg.text);
    } else {
      // Wiadomość od innego klienta
      printf("Klient %d: %s\n", msg.client_id, msg.text);
    }
  }
}

int main() {
  key_t client_key;

  // Rejestracja obsługi sygnałów
  signal(SIGINT, handle_signal);
  signal(SIGTERM, handle_signal);

  // Generowanie unikalnego klucza dla kolejki klienta
  srand(time(NULL) ^ getpid());
  client_key = ftok("/tmp", rand() % 256);

  // Utworzenie kolejki klienta
  client_queue_id = msgget(client_key, IPC_CREAT | 0666);
  if (client_queue_id == -1) {
    perror("Błąd podczas tworzenia kolejki klienta");
    exit(1);
  }

  printf("Utworzono kolejkę klienta. ID: %d, Klucz: %d\n", client_queue_id,
         client_key);

  // Otwarcie kolejki serwera
  server_queue_id = msgget(SERVER_KEY, 0);
  if (server_queue_id == -1) {
    perror("Błąd podczas otwierania kolejki serwera");
    cleanup();
    exit(1);
  }

  // Przygotowanie komunikatu inicjalizacyjnego
  ClientMessage init_msg;
  init_msg.mtype = INIT;
  init_msg.client_queue = client_key;
  strcpy(init_msg.text, "INIT");

  // Wysłanie komunikatu inicjalizacyjnego do serwera
  if (msgsnd(server_queue_id, &init_msg, sizeof(ClientMessage) - sizeof(long),
             0) == -1) {
    perror("Błąd podczas wysyłania komunikatu inicjalizacyjnego");
    cleanup();
    exit(1);
  }

  // Oczekiwanie na odpowiedź serwera
  ServerMessage response;
  if (msgrcv(client_queue_id, &response, sizeof(ServerMessage) - sizeof(long),
             1, 0) == -1) {
    perror("Błąd podczas odbierania odpowiedzi serwera");
    cleanup();
    exit(1);
  }

  // Odczytanie przydzielonego ID klienta
  client_id = response.client_id;
  printf("%s\n", response.text);

  // Utworzenie procesu potomnego do odbierania komunikatów
  child_pid = fork();

  if (child_pid == -1) {
    perror("Błąd podczas tworzenia procesu potomnego");
    cleanup();
    exit(1);
  } else if (child_pid == 0) {
    // Proces potomny - odbieranie komunikatów
    signal(SIGINT, SIG_DFL); // Przywrócenie domyślnej obsługi sygnału
    receive_messages();
    exit(0);
  } else {
    // Proces główny - wysyłanie komunikatów
    char buffer[MAX_MSG_SIZE];
    ClientMessage msg;
    msg.mtype = MESSAGE;
    msg.client_id = client_id;

    printf("Wpisz wiadomość i naciśnij Enter (Ctrl+C, aby zakończyć):\n");

    while (1) {
      // Odczytanie wiadomości z wejścia standardowego
      if (fgets(buffer, MAX_MSG_SIZE, stdin) == NULL) {
        break;
      }

      // Usunięcie znaku nowej linii
      buffer[strcspn(buffer, "\n")] = '\0';

      // Sprawdzenie, czy wiadomość nie jest pusta
      if (strlen(buffer) > 0) {
        // Wysłanie komunikatu do serwera
        strncpy(msg.text, buffer, MAX_MSG_SIZE);

        if (msgsnd(server_queue_id, &msg, sizeof(ClientMessage) - sizeof(long),
                   0) == -1) {
          perror("Błąd podczas wysyłania komunikatu");
        }
      }
    }

    // Zakończenie pracy klienta
    cleanup();
  }

  return 0;
}