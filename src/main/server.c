#if PROFILE == prod || PROFILE == dev

#include <ctype.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include "http.h"
#include "json.h"

// Http {{{
#define HTTP_REQUEST_PATH_MAX_SIZE (1 << 8)

#define HTTP_HEADER_MAX_COUNT (1 << 8)
#define HTTP_HEADER_KEY_MAX_SIZE (1 << 8)
#define HTTP_HEADER_VALUE_MAX_SIZE (1 << 8)

#define HTTP_SERVER_PORT atoi(getenv("HTTP_SERVER_PORT"))

// Server {{{
static volatile sig_atomic_t stop_server = 0;
void SIGINT_handler(int _);
void handle_request(FILE *req, FILE *resp, HTTPHandler handler);
int http_echo_request_as_json(struct HTTPRequest *req, struct HTTPResponse *resp);

int main() {
  char str[256];
  const char *test = str;
  stop_server = 0;
  signal(SIGINT, SIGINT_handler);
  int err, server_sck, session_sck, read_size;
  struct sockaddr_in addr;
  socklen_t addrlen;
  if ((server_sck = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket creation failure");
    exit(EXIT_FAILURE);
  }
  addr.sin_family = AF_INET;
  addr.sin_addr.s_addr = INADDR_ANY;
  addr.sin_port = htons(HTTP_SERVER_PORT);
  addrlen = sizeof(addr);
  if ((err = bind(server_sck, (const struct sockaddr *)&addr, sizeof(addr))) !=
      EXIT_SUCCESS) {
    perror("socket bind failure");
    exit(EXIT_FAILURE);
  }
  if ((err = listen(server_sck, 0)) != EXIT_SUCCESS) {
    perror("socket listen failure");
    exit(EXIT_FAILURE);
  }
  printf("Server started on port %i...\n", HTTP_SERVER_PORT);
  while (!stop_server &&
         (session_sck =
              accept(server_sck, (struct sockaddr *)&addr, &addrlen)) >= 0) {
    FILE *req = fdopen(session_sck, "r");
    FILE *resp = fdopen(session_sck, "a");
    handle_request(req, resp, http_echo_request_as_json);
    fflush(resp);
    fclose(req);
    fclose(resp);
    close(session_sck);
  }
  perror("server exited");
  close(server_sck);
}

int http_echo_request_as_json(struct HTTPRequest *req, struct HTTPResponse *resp) {
  int header_count;
  char *header_keys[100];
  const char *path = HTTPRequest_path(req),
             *method = HTTPMethod_word(HTTPRequest_method(req));
  JSON req_json = JSON_object(), headers_json = JSON_object();
  JSON_object_set(req_json, "path", JSON_string(path));
  JSON_object_set(req_json, "method", JSON_string(method));
  header_count = HTTPRequest_headers(req, header_keys);
  for (int i = 0; i < header_count; i++) {
    const char *header_key = header_keys[i];
    const char *header_value = HTTPRequest_getheader(req, header_key);
    JSON_object_set(headers_json, header_key, JSON_string(header_value));
  }
  JSON_object_set(req_json, "headers", headers_json);
  FILE *resp_body = HTTPResponse_body(resp);
  HTTPResponse_setstatus(resp, HTTP_OK);
  JSON_fwrite(req_json, resp_body);
  fflush(resp_body);
  JSON_free(req_json);
  return 0;
}

void handle_request(FILE *req, FILE *resp, HTTPHandler handler) {
  struct HTTPRequest *http_req = HTTPRequest_new();
  struct HTTPResponse *httpr_resp = HTTPResponse_new();
  if (!HTTPRequest_fscan(req, http_req)) {
    HTTPResponse_setstatus(httpr_resp, HTTP_BAD_REQUEST);
    return;
  }
  int err = handler(http_req, httpr_resp);
  if (err) {
    HTTPResponse_setstatus(httpr_resp, HTTP_INTERNAL_SERVER_ERROR);
    return;
  }
  HTTPRequest_free(http_req);
  HTTPResponse_free(httpr_resp);
}

void SIGINT_handler(int _) {
  if (!stop_server) {
    printf("graceful shutdown\n");
    stop_server = 1;
  } else {
    exit(EXIT_FAILURE);
  }
}

// }}}

#endif

// vim: fdm=marker
