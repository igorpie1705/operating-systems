#include <errno.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_CLIENTS 10
#define MAX_MSG_SIZE 256
#define SERVER_KEY 12345

// Typy komunikatów
#define INIT 1
#define MESSAGE 2

// Struktura klienta
typedef struct {
  int id;       // Identyfikator klienta
  int queue_id; // ID kolejki komunikatów klienta
  int active;   // Czy klient jest aktywny
} Client;

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

// Tablica klientów
Client clients[MAX_CLIENTS];
int server_queue_id;

// Obsługa zakończenia programu
void cleanup() {
  // Usunięcie kolejki serwera
  if (msgctl(server_queue_id, IPC_RMID, NULL) == -1) {
    perror("Błąd podczas usuwania kolejki serwera");
  }
  printf("Serwer zakończył działanie.\n");
  exit(0);
}

// Obsługa sygnałów
void handle_signal(int sig) {
  printf("\nOtrzymano sygnał %d. Kończenie pracy serwera...\n", sig);
  cleanup();
}

int main() {
  // Inicjalizacja tablicy klientów
  for (int i = 0; i < MAX_CLIENTS; i++) {
    clients[i].active = 0;
  }

  // Rejestracja obsługi sygnałów
  signal(SIGINT, handle_signal);
  signal(SIGTERM, handle_signal);

  // Utworzenie kolejki serwera
  server_queue_id = msgget(SERVER_KEY, IPC_CREAT | 0666);
  if (server_queue_id == -1) {
    perror("Błąd podczas tworzenia kolejki serwera");
    exit(1);
  }

  printf("Serwer uruchomiony. ID kolejki: %d\n", server_queue_id);

  // Główna pętla serwera
  while (1) {
    ClientMessage client_msg;
    ServerMessage server_msg;

    // Odbieranie komunikatu od klienta
    if (msgrcv(server_queue_id, &client_msg,
               sizeof(ClientMessage) - sizeof(long), 0, 0) == -1) {
      perror("Błąd podczas odbierania komunikatu");
      continue;
    }

    // Obsługa inicjalizacji nowego klienta
    if (client_msg.mtype == INIT) {
      int client_id = -1;

      // Znajdź wolne miejsce w tablicy klientów
      for (int i = 0; i < MAX_CLIENTS; i++) {
        if (!clients[i].active) {
          client_id = i;
          break;
        }
      }

      if (client_id == -1) {
        // Brak miejsca dla nowego klienta
        printf("Osiągnięto maksymalną liczbę klientów (%d)\n", MAX_CLIENTS);
        continue;
      }

      // Otwórz kolejkę klienta
      int client_queue_id = msgget(client_msg.client_queue, 0);
      if (client_queue_id == -1) {
        perror("Błąd podczas otwierania kolejki klienta");
        continue;
      }

      // Zapisz informacje o kliencie
      clients[client_id].id = client_id;
      clients[client_id].queue_id = client_queue_id;
      clients[client_id].active = 1;

      // Wyślij potwierdzenie do klienta z jego ID
      server_msg.mtype = 1; // Typ komunikatu dla klienta
      server_msg.client_id = client_id;
      sprintf(server_msg.text, "Połączono z serwerem. Twój ID: %d", client_id);

      if (msgsnd(client_queue_id, &server_msg,
                 sizeof(ServerMessage) - sizeof(long), 0) == -1) {
        perror("Błąd podczas wysyłania potwierdzenia");
        clients[client_id].active = 0;
        continue;
      }

      printf("Nowy klient połączony. ID: %d, Kolejka: %d\n", client_id,
             client_queue_id);
    } else if (client_msg.mtype == MESSAGE) {
      // Obsługa komunikatu do przekazania innym klientom
      int sender_id = client_msg.client_id;

      printf("Klient %d: %s\n", sender_id, client_msg.text);

      // Przekaż komunikat do wszystkich pozostałych klientów
      server_msg.mtype = 1; // Typ komunikatu dla klientów
      server_msg.client_id = sender_id;
      strncpy(server_msg.text, client_msg.text, MAX_MSG_SIZE);

      for (int i = 0; i < MAX_CLIENTS; i++) {
        if (clients[i].active && i != sender_id) {
          if (msgsnd(clients[i].queue_id, &server_msg,
                     sizeof(ServerMessage) - sizeof(long), 0) == -1) {
            // Sprawdź, czy kolejka jeszcze istnieje
            if (errno == EINVAL || errno == EIDRM) {
              printf("Klient %d odłączony (kolejka nie istnieje)\n", i);
              clients[i].active = 0;
            } else {
              perror("Błąd podczas wysyłania komunikatu do klienta");
            }
          }
        }
      }
    }
  }

  return 0;
}