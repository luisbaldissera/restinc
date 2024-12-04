#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>

#define BUFFER_SIZE      1024
#define HTTP_SERVER_PORT atoi(getenv("HTTP_SERVER_PORT"))

enum HTTP_Method {
  HTTP_METHOD_GET,
  HTTP_METHOD_POST,
  HTTP_METHOD_PUT,
  HTTP_METHOD_OPTIONS,
  HTTP_METHOD_PATCH,
  HTTP_METHOD_HEAD,
  HTTP_METHOD_DELETE,
  HTTP_METHOD_CONNECT,
  HTTP_METHOD_TRACE
};

struct HTTP_Request {
  enum HTTP_Method method;
  char path[BUFFER_SIZE];
  void *headers;
  FILE *body;
};

struct HTTP_Response {
  int status;
  void *headers;
  FILE *body;
};

static volatile sig_atomic_t stop_server = 0;
void SIGINT_handler(int _);
void handle(FILE *req, FILE *resp);
void HTTP_handle(struct HTTP_Request *req, struct HTTP_Response *resp);

int main() {
    stop_server = 0;
  signal(SIGINT, SIGINT_handler);
  int err, server_sck, session_sck, read_size;
  struct sockaddr_in addr;
  char buf[BUFFER_SIZE], method[16], path[256], proto[16];
  socklen_t addrlen;
  if ((server_sck = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket creation failure");
    exit(EXIT_FAILURE);
  }
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(HTTP_SERVER_PORT);
  addrlen = sizeof(addr);
  if((err = bind(server_sck, (const struct sockaddr *) &addr, sizeof(addr))) != EXIT_SUCCESS) {
    perror("socket bind failure");
    exit(EXIT_FAILURE);
  }
  if ((err = listen(server_sck, 0)) != EXIT_SUCCESS) {
    perror("socket listen failure");
    exit(EXIT_FAILURE);
  }
  printf("Server started on port %i...\n", HTTP_SERVER_PORT);
  while (!stop_server && (session_sck = accept(server_sck, (struct sockaddr *) &addr, &addrlen)) >= 0) {
    printf("handling request\n");
    FILE *req = fdopen(session_sck, "r");
    FILE *resp = fdopen(session_sck, "a");
    handle(req, resp);
    fflush(resp);
    fclose(req);
    fclose(resp);
    close(session_sck);
  }
  perror("server exited");
  close(server_sck);
}

void handle(FILE *req, FILE *resp) {

}

void SIGINT_handler(int _) {
  if (!stop_server) {
    printf("graceful shutdown\n");
    stop_server = 1;
  } else {
    exit(EXIT_FAILURE);
  }
}
